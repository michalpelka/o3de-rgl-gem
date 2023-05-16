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
#pragma once

#include <Lidar/RaycastResults.h>
#include <ROS2/Lidar/LidarRaycasterBus.h>
#include <rgl/api/core.h>

namespace RGL
{
    class LidarRaycaster : protected ROS2::LidarRaycasterRequestBus::Handler
    {
    public:
        explicit LidarRaycaster(const AZ::Uuid& uuid);
        LidarRaycaster(LidarRaycaster&& lidarSystem) noexcept;
        LidarRaycaster(const LidarRaycaster& lidarSystem) = default;
        ~LidarRaycaster() override;

    protected:
        // LidarRaycasterRequestBus overrides
        void ConfigureRayOrientations(const AZStd::vector<AZ::Vector3>& orientations) override;
        void ConfigureRayRange(float range) override;
        void ConfigureMinimumRayRange(float range) override;
        void ConfigureRaycastResultFlags(ROS2::RaycastResultFlags flags) override;

        ROS2::RaycastResult PerformRaycast(const AZ::Transform& lidarTransform) override;

        void ConfigureNoiseParameters(
            [[maybe_unused]] float angularNoiseStdDev,
            [[maybe_unused]] float distanceNoiseStdDevBase,
            [[maybe_unused]] float distanceNoiseStdDevRisePerMeter) override;
        void ExcludeEntities(const AZStd::vector<AZ::EntityId>& excludedEntities) override;
        void ConfigureMaxRangePointAddition(bool addMaxRangePoints) override;

    private:
        bool m_addMaxRangePoints{ false }, m_useCompact{ true };
        AZ::Uuid m_uuid;
        float m_range{ 1.0f };
        float m_minRange{ 0.0f };
        ROS2::RaycastResultFlags m_resultFlags{ ROS2::RaycastResultFlags::Points };

        AZStd::vector<rgl_field_t> m_resultFields = { RGL_FIELD_IS_HIT_I32, RGL_FIELD_XYZ_F32 };
        AZStd::vector<AZ::Matrix3x4> m_rayTransforms{ AZ::Matrix3x4::CreateIdentity() };
        AZStd::vector<float> m_rayDirections{ 1.0f };

        RaycastResults m_rglRaycastResults;
        ROS2::RaycastResult m_raycastResults;

        rgl_node_t m_rayPosesNode{ nullptr }, m_lidarTransformNode{ nullptr }, m_angularNoiseNode{ nullptr }, m_rayTraceNode{ nullptr },
            m_distanceNoiseNode{ nullptr }, m_pointsCompactNode{ nullptr }, m_pointsFormatNode{ nullptr };

        void ConfigurePointsCompact(bool value = true);
    };
} // namespace RGL
