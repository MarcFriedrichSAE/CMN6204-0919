#region using
using UnrealBuildTool;
#endregion

/// <summary>
/// project class
/// </summary>
public class WaveShooterVR : ModuleRules
{
    #region constrctor
    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="Target">target rules</param>
    public WaveShooterVR(ReadOnlyTargetRules Target) : base(Target)
    {
        // set pch usage
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // add public modules
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        // add private modules
        PrivateDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay" });
    }
    #endregion
}