#region using
using UnrealBuildTool;
using System.Collections.Generic;
#endregion

/// <summary>
/// project editor target class
/// </summary>
public class WaveShooterVREditorTarget : TargetRules
{
    #region constructor
    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="Target">target info</param>
    public WaveShooterVREditorTarget(TargetInfo Target) : base(Target)
    {
        // set type to editor
        Type = TargetType.Editor;

        // add modules
        ExtraModuleNames.AddRange(new string[] { "WaveShooterVR" });
    }
    #endregion
}