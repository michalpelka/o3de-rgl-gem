# O3DE RGL Gem
The O3DE Robotec GPU Lidar Gem is a subgem of the [O3DE ROS2 Gem](https://github.com/o3de/o3de-extras/tree/development/Gems/ROS2) providing it with a fast and scalable LiDAR implementation by making use of the [Robotec GPU Lidar](https://github.com/RobotecAI/RobotecGPULidar) library.

## Features
Combined with the O3DE ROS2 Gem's `Lidar Sensor Component` the O3DE RGL Gem allows for creation of a configurable, high - performance LiDAR. The Gem provides a faithful representation of the simulated environment by supporting the following visuals:
- Mesh Component
- Terrain created using the O3DE Terrain Gem

<img src="static/gif/rgl_gem_preview1.gif" alt="drawing" width="500"/>

You can fully customize the LiDAR's settings using the O3DE Level Editor. Those include properties like:
- configurable raycasting pattern
- lidar range
- entities excluded from raycasting

You can also choose one of the presets provided by the ROS2 Gem to create a LiDAR model that fits your needs.

<img src="static/gif/rgl_gem_preview2.gif" alt="drawing" width="500"/>

## Requirements
- [Runtime requirements of the Robotec GPU Lidar](https://github.com/RobotecAI/RobotecGPULidar#runtime-requirements).
- Any O3DE project with the [O3DE ROS2 Gem](https://github.com/o3de/o3de-extras/tree/development/Gems/ROS2) enabled.

## Setup
1. **Clone the Gem's repository.**
    ```bash
    git clone # TO DO - add link once the gem is moved.
    ```
2. **Register the Gem.** \
    You can either register the gem through the Command Line Interface or the O3DE Project Manager:
    - **CLI** \
        Head to your local O3DE engine directory (*o3de-dir*) and register the gem using it's path (*gem-path*).
        ```bash
        cd <o3de-dir>
        ./scripts/o3de.sh register --gem-path <gem-path>
        ```
    - **Project Manager** \
        Open the Project Manager. Select **Gems -> Add Existing Gem**. Locate the gem's directory and select **Choose**.

3. **Enable the Gem in your project.** \
    Once again you can either enable it through the Command Line Interface or the O3DE Project Manager:

    ***Note:*** *Please, make sure to enable the ROS2 Gem first.*

    - **CLI** \
        In your local o3de engine directory you can enable the gem for your project (*project-path*).
        ```bash
        ./scripts/o3de.sh enable-gem -gn ROS2 -pp <project-path>
        ```
    - **Project Manager** \
        Open your project. Select **File -> Edit Project Settings -> Configure Gems**. Now, search for the Robotec GPU Lidar Gem and enable it.

## Usage
1. **Create an entity with a `ROS2 Lidar Sensor` component.**
    
    Within your O3DE project add a new entity by right - clicking on the viewport and selecting **Create entity**.
    
    <img src="static/png/usage_instruction1.png" alt="drawing" width="400"/>

    Select the newly created entity within the Entity Outliner. Next, within the Entity Inspector select **Add Component**. Then, search for `ROS2 Lidar Sensor` and add it to your entity using the left mouse button.
    
    <img src="static/png/usage_instruction2.png" alt="drawing" width="400"/>

    ***Note:** You need to add the required `ROS2 Frame` component as well.*

2. **Select `RobotecGPULidar` as your LiDAR implementation.**

    In the Entity Inspector find the `ROS2 Lidar Sensor` component and change the **Lidar Implementation** to `RobotecGPULidar`.

    <img src="static/png/usage_instruction3.png" alt="drawing" width="400"/>

    ***Note:** If you do not see the `RobotecGPULidar` implementation, please make sure you followed the **Setup** instructions correctly.*
3. **Customize your LiDAR.**

    After following through all previous instructions, you can customize the `ROS2 Lidar Sensor` component in the Entity Inspector to fit all your needs.