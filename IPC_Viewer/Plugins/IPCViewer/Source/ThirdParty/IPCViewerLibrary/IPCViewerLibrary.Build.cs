// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class IPCViewerLibrary : ModuleRules
{
	public IPCViewerLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            // Add the import library
            PublicLibraryPaths.Add(Path.Combine(ModuleDirectory, "x64", "Release"));
            PublicAdditionalLibraries.Add("IPC_Plugin.lib");

            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("IPC_Plugin.dll");
        }
    }
}
