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

    private companion object {
        private fun StripFileNameExtension(fileName: String): String
        {
            val idx = fileName.indexOf('.')
            if (idx == -1) {
                return fileName
            }
            return fileName.substring(idx + 1)
        }

        /** Get last component of fully qualified module name. */
        private fun GetFileNameExtension(moduleName: String): String
        {
            val idx = moduleName.indexOf('.')
            if (idx == -1) {
                return moduleName
            }
            return moduleName.substring(0, idx)
        }

        private fun GetSubmoduleName(moduleName: String): String = GetFileNameExtension(moduleName)
    }

    /** Indexes file names both with and without extension. */
    private class FileNameSet {
        val fileNames = TreeSet<String>()
        /** Key is basename, value is extension. */
        val baseNames = TreeMap<String, String>()

        fun Add(fileName: String)
        {
            val baseName = StripFileNameExtension(fileName)
            val ext = baseNames[baseName]
            if (ext != null) {
                throw Error("Several files have the same basename in one directory which is " +
                            "disallowed: `$fileName` and `$baseName.$ext`")
            }
            fileNames.add(fileName)
            baseNames[baseName] = GetFileNameExtension(fileName)
        }

        fun Remove(fileName: String)
        {
            fileNames.remove(fileName)
            baseNames.remove(StripFileNameExtension(fileName))
        }

        fun GetFileName(baseName: String): String?
        {
            val ext = baseNames[baseName] ?: return null
            return "$baseName.$ext"
        }
    }

    private val modules = TreeMap<String, ModuleNode>()

    private fun ScanDirectory(dirPath: File, implicitModuleName: String?,
                              moduleScriptReader: ModuleScriptReader)
    {
        val moduleScript = moduleScriptReader.Read(dirPath)
        val dirs = TreeSet<String>()
        val implFiles = FileNameSet()
        val ifaceFiles = FileNameSet()

        for (fileName in dirPath.list() ?: emptyArray()) {
            val path = dirPath.resolve(fileName)
            if (path.isDirectory) {
                dirs.add(fileName)
                continue
            }
            for (ext in adkConfig.cppImplExt) {
                if (path.isFile && fileName.endsWith(ext)) {
                    implFiles.Add(fileName)
                    continue
                }
            }
            for (ext in adkConfig.cppModuleIfaceExt) {
                if (path.isFile && fileName.endsWith(ext)) {
                    ifaceFiles.Add(fileName)
                    continue
                }
            }
        }

        fun SetModuleDefaultFiles(module: ModuleNode, name: String)
        {
            val baseName = GetSubmoduleName(name)
            ifaceFiles.GetFileName(baseName)?.also {
                module.SetIfaceFile(dirPath.resolve(it))
                ifaceFiles.Remove(it)
            }
            implFiles.GetFileName(baseName)?.also {
                module.AddImplFile(dirPath.resolve(it))
                implFiles.Remove(it)
            }
        }

        /* Default module is one represented by this directory. There may be other modules implied
        * by module interface files and optionally configured by named module block in the module
        * script.
        */
        val defaultModuleName =
            if (moduleScript != null && moduleScript.nameProp.prop.isPresent) {
                moduleScript.name
            } else if (implicitModuleName != null) {
                implicitModuleName
            } else {
                throw Error("Module name should be specified for root directory $dirPath")
            }
        val defaultModule = ModuleNode(defaultModuleName)
        if (moduleScript != null) {
            defaultModule.Configure(moduleScript)
        }
        SetModuleDefaultFiles(defaultModule, defaultModuleName)

        //XXX check all left interface files, create modules for them, add whatever left
        // implementation files to the default module.
    }
}