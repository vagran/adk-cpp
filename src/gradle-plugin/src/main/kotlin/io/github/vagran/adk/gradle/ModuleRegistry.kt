package io.github.vagran.adk.gradle

import java.io.File
import java.util.*

class ModuleRegistry(private val adkConfig: AdkExtension) {

    fun interface ModuleScriptReader {
        /**
         * @param dirPath Directory to check for module script.
         * @return Extension context with all read data, or null no script file found.
         */
        fun Read(dirPath: File): ModuleExtensionContext?
    }

    fun Build(moduleScriptReader: ModuleScriptReader)
    {
        for (modPath in adkConfig.modules) {
            ScanDirectory(modPath, null, moduleScriptReader)
        }
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////

    private val modules = TreeMap<String, ModuleNode>()

    private fun ScanDirectory(dirPath: File, implicitModuleName: String?,
                              moduleScriptReader: ModuleScriptReader)
    {
        val moduleScript = moduleScriptReader.Read(dirPath)

    }
}