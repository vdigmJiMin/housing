using UnrealBuildTool;

public class TIHHousingCore : ModuleRules
{
    public TIHHousingCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "GameplayTags","GameplayStateTreeModule",
                "StructUtils",
                "SQLiteCore","SQLiteSupport", 
                "InputCore", "EnhancedInput",
                "Json","JsonUtilities",
                "AIModule",
                "MeshModelingToolsExp",
                "InteractiveToolsFramework",  // 추가
                "UMG",
                "UMGEditor",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
            }
        );
    }
}