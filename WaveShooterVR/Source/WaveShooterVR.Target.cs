#region using
using UnrealBuildTool;
using System.Collections.Generic;
#endregion

/// <summary>
/// project game target class
/// </summary>
public class WaveShooterVRTarget : TargetRules
{
    #region constructor
    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="Target">target info</param>
    public WaveShooterVRTarget(TargetInfo Target) : base(Target)
    {
        // set type to game
        Type = TargetType.Game;

        // add modules
        ExtraModuleNames.AddRange(new string[] { "WaveShooterVR" });
    }
    #endregion
}