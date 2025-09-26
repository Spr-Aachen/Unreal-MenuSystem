<div align = "center">

# Unreal MenuSystem Plugin

A multiplayer plugin with menu system for Unreal Engine 5

![Cover](/docs/media/Cover.png)

</div>


## Guide

### 1. Setup Project

1. Create a Unreal Engine 5 C++ project if you don't have any

2. Open the editor, go to `Edit` -> `Plugins`, search for "Online Subsystem Steam", enable the plugin, then close the editor

3. Navigate to the 'Config' folder, open 'DefaultEngine.ini' and paste the following content:
    ```
    [/Script/Engine.GameEngine]
    +NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")

    [OnlineSubsystem]
    DefaultPlatformService=Steam

    [OnlineSubsystemSteam]
    bEnabled=true
    SteamDevAppId=480 ; This is the dev app ID for Steam's example project called Space War
    bInitServerOnClient=true ; Set to true if using Sessions

    [/Script/OnlineSubsystemSteam.SteamNetDriver]
    NetConnectionClassName="OnlineSubsystemSteam.SteamNetConnection"
    ```

4. Open 'DefaultGame.ini' and paste the following content
    ```
    [/Script/Engine.GameSession]
    MaxPlayers = 100 ; Set to the max number of players for game project we would like to have
    ```

### 2. Fetch Plugin

1. Navigate to your target project's directory

2. Clone the repository or add as a submodule
    - Clone
        ```shell
        git clone https://github.com/Spr-Aachen/Unreal-MenuSystem.git ./Plugins/MenuSystem
        ```
    - Add as a submodule
        ```shell
        git submodule add https://github.com/Spr-Aachen/Unreal-MenuSystem.git ./Plugins/MenuSystem
        ```

### 3. Build Project

1. Launch the .uproject file, if asking you whether to rebuild missing modules, select "Yes"

2. Go to `Tools` -> `Generate/Refresh Visual Studio Code Project`


## Cases

Here are some projects based on Unreal MenuSystem Plugin:
- [Unreal-MenuSystem-Demo](https://github.com/Spr-Aachen/Unreal-MenuSystem-Demo)
- [Unreal-BlasterProject](https://github.com/Spr-Aachen/Unreal-BlasterProject)