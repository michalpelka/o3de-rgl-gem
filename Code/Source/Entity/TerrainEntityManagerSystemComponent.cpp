/* Copyright 2020-2021, Robotec.ai sp. z o.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <Entity/TerrainEntityManagerSystemComponent.h>
#include <Utilities/RGLUtils.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace RGL
{
    TerrainEntityManagerSystemComponent::~TerrainEntityManagerSystemComponent()
    {
        EnsureManagedEntityDestroyed();
    }

    void TerrainEntityManagerSystemComponent::Init()
    {
    }

    void TerrainEntityManagerSystemComponent::Activate()
    {
        UpdateWorldBounds();
        AzFramework::Terrain::TerrainDataNotificationBus::Handler::BusConnect();
    }

    void TerrainEntityManagerSystemComponent::Deactivate()
    {
        AzFramework::Terrain::TerrainDataNotificationBus::Handler::BusDisconnect();
    }

    void TerrainEntityManagerSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<TerrainEntityManagerSystemComponent, AZ::Component>()->Version(0);

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<TerrainEntityManagerSystemComponent>("Terrain Entity Manager", "Manages the RGL Terrain entity.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                    ->Attribute(AZ::Edit::Attributes::Category, "RGL")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }

    void TerrainEntityManagerSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("TerrainEntityManagerService"));
    }

    void TerrainEntityManagerSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("TerrainEntityManagerService"));
    }

    void TerrainEntityManagerSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("RGLService"));
    }

    void TerrainEntityManagerSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void TerrainEntityManagerSystemComponent::EnsureManagedEntityDestroyed()
    {
        if (m_rglEntity)
        {
            RGL_CHECK(rgl_entity_destroy(m_rglEntity));
            m_rglEntity = nullptr;
        }

        if (m_rglMesh)
        {
            RGL_CHECK(rgl_mesh_destroy(m_rglMesh));
            m_rglMesh = nullptr;
        }
    }

    void TerrainEntityManagerSystemComponent::UpdateWorldBounds()
    {
        AZ::Aabb newWorldBounds = AZ::Aabb::CreateFromPoint(AZ::Vector3::CreateZero());
        AzFramework::Terrain::TerrainDataRequestBus::BroadcastResult(
            newWorldBounds, &AzFramework::Terrain::TerrainDataRequests::GetTerrainAabb);

        const AZ::Vector3 newWorldMin = newWorldBounds.GetMin();
        const AZ::Vector3 newWorldSize = newWorldBounds.GetExtents();

        if (newWorldBounds == m_currentWorldBounds || newWorldSize.GetX() < SectorSideLength || newWorldSize.GetY() < SectorSideLength)
        {
            return; // No need to update.
        }

        m_currentWorldBounds = newWorldBounds;
        const auto sectorCountX = aznumeric_cast<size_t>(newWorldSize.GetX() / SectorSideLength);
        const auto sectorCountY = aznumeric_cast<size_t>(newWorldSize.GetY() / SectorSideLength);

        // To create a mesh constructed out of n * m sectors we need (n + 1) * (m + 1) vertices.
        // Each vertex is positioned in one of the sector's corners.
        size_t vertexCountX = sectorCountX + 1LU;
        size_t vertexCountY = sectorCountY + 1LU;

        m_vertices.clear();
        m_vertices.reserve(vertexCountX * vertexCountY);
        for (size_t vertexIndexX = 0LU; vertexIndexX < vertexCountX; ++vertexIndexX)
        {
            for (size_t vertexIndexY = 0LU; vertexIndexY < vertexCountY; ++vertexIndexY)
            {
                m_vertices.emplace_back(rgl_vec3f{
                    newWorldMin.GetX() + aznumeric_cast<float>(vertexIndexX) * SectorSideLength,
                    newWorldMin.GetY() + aznumeric_cast<float>(vertexIndexY) * SectorSideLength,
                    0.0f,
                });
            }
        }

        m_indices.clear();
        m_indices.reserve(sectorCountX * sectorCountY * TrianglesPerSector);
        for (size_t sectorIndexX = 0LU; sectorIndexX < sectorCountX; ++sectorIndexX)
        {
            for (size_t sectorIndexY = 0LU; sectorIndexY < sectorCountY; ++sectorIndexY)
            {
                const auto upperLeft = aznumeric_cast<int32_t>(sectorIndexY * vertexCountX + sectorIndexX);
                const auto upperRight = aznumeric_cast<int32_t>(upperLeft + 1LU);
                const auto lowerLeft = aznumeric_cast<int32_t>(upperLeft + vertexCountX);
                const auto lowerRight = aznumeric_cast<int32_t>(upperRight + vertexCountX);

                m_indices.emplace_back(rgl_vec3i{ upperLeft, upperRight, lowerLeft });
                m_indices.emplace_back(rgl_vec3i{ lowerLeft, upperRight, lowerRight });
            }
        }

        EnsureManagedEntityDestroyed();

        Utils::SafeRglMeshCreate(m_rglMesh, m_vertices.data(), m_vertices.size(), m_indices.data(), m_indices.size());
        if (!m_rglMesh)
        {
            AZ_Assert(false, "The TerrainEntityManager was unable to create an RGL mesh.");
            return;
        }

        Utils::SafeRglEntityCreate(m_rglEntity, m_rglMesh);
        if (!m_rglEntity)
        {
            AZ_Assert(false, "The TerrainEntityManager was unable to create an RGL entity.");
            return;
        }

        RGL_CHECK(rgl_entity_set_pose(m_rglEntity, &Utils::IdentityTransform));
    }

    void TerrainEntityManagerSystemComponent::UpdateDirtyRegion(const AZ::Aabb& dirtyRegion)
    {
        const AZ::Aabb dirtyRegion2D = AZ::Aabb::CreateFromMinMaxValues(
            dirtyRegion.GetMin().GetX(),
            dirtyRegion.GetMin().GetY(),
            AZStd::numeric_limits<float>::lowest(),
            dirtyRegion.GetMax().GetX(),
            dirtyRegion.GetMax().GetY(),
            AZStd::numeric_limits<float>::max());

        for (rgl_vec3f& vertex : m_vertices)
        {
            if (dirtyRegion2D.Contains(Utils::AzVector3FromRglVec3f(vertex)))
            {
                float height = AZStd::numeric_limits<float>::lowest();
                bool terrainExists = false;
                AzFramework::Terrain::TerrainDataRequestBus::BroadcastResult(
                    height,
                    &AzFramework::Terrain::TerrainDataRequestBus::Events::GetHeightFromFloats,
                    vertex.value[0],
                    vertex.value[1],
                    AzFramework::Terrain::TerrainDataRequests::Sampler::BILINEAR,
                    &terrainExists);

                if (height != AZStd::numeric_limits<float>::lowest() && terrainExists)
                {
                    vertex.value[2] = height;
                }
            }
        }

        RGL_CHECK(rgl_mesh_update_vertices(m_rglMesh, m_vertices.data(), aznumeric_cast<int32_t>(m_vertices.size())));
    }

    void TerrainEntityManagerSystemComponent::OnTerrainDataChanged(const AZ::Aabb& dirtyRegion, TerrainDataChangedMask dataChangedMask)
    {
        if ((dataChangedMask & TerrainDataChangedMask::Settings) != TerrainDataChangedMask::None)
        {
            UpdateWorldBounds();
        }

        if ((dataChangedMask & TerrainDataChangedMask::HeightData) != TerrainDataChangedMask::None)
        {
            UpdateDirtyRegion(dirtyRegion);
        }
    }
} // namespace RGL
