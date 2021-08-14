package io.github.vagran.adk.gradle

import java.io.File

/** Represents build plan with all artifacts, their dependencies and required actions to produce
 * each one.
 */
class BuildTree(private val adkConfig: AdkExtension) {

    val buildDir: File get() = adkConfig.project.buildDir.resolve(adkConfig.buildType)

    fun Build(moduleRegistry: ModuleRegistry)
    {
        Builder(moduleRegistry).Build()
        //XXX
    }

    fun GetObjBuildDirectory(sourceDirPath: File): File
    {
        val projectDir = adkConfig.project.projectDir.normalize().toPath()
        val objDir = buildDir.resolve("obj").toPath()
        val srcDir = sourceDirPath.normalize().toPath()
        return if (srcDir.startsWith(projectDir)) {
            val relPath = projectDir.relativize(srcDir)
            objDir.resolve(relPath).toFile()
        } else {
            objDir.resolve("__external").resolve(srcDir.subpath(0, srcDir.nameCount)).toFile()
        }
    }

    fun GetObjBuildPath(sourcePath: File, replaceExtension: String? = null): File
    {
        val dir = GetObjBuildDirectory(sourcePath.parentFile)
        val name = if (replaceExtension != null) {
            sourcePath.nameWithoutExtension + "." + replaceExtension
        } else {
            sourcePath.name
        }
        return dir.resolve(name)
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////

    private inner class Builder(private val moduleRegistry: ModuleRegistry) {

        private val compilerInfo = CompilerInfo(adkConfig)

        fun Build()
        {
            if (adkConfig.binType != BinType.APP.value) {
                throw Error("Binary type other than application is not yet supported")
            }

            val modules = ModuleRegistry.GatherAllDependencies(moduleRegistry.mainModules)
            val binRecipe = CppAppExecutableRecipe(compilerInfo, modules)
            val binFile = ExecutableFileNode(buildDir.resolve(adkConfig.binName), binRecipe)
            for (mainModule in moduleRegistry.mainModules) {
                binFile.dependencies.addAll(ProcessModule(mainModule))
            }
        }

        /** @return Nodes produces by the specified module. */
        private fun ProcessModule(module: ModuleNode): Iterable<BuildNode>
        {
            val modules = ModuleRegistry.GatherAllDependencies(module)
            val resultNodes = ArrayList<BuildNode>()

            val depNodes = ArrayList<BuildNode>()
            module.dependNodes.forEach {
                depModule ->
                depNodes.addAll(ProcessModule(depModule))
            }

            fun AddNode(node: BuildNode) {
                resultNodes.add(node)
                node.dependencies.addAll(depNodes)
            }

            module.ifaceFile?.also {
                ifaceFile ->
                val compiledModule = CppCompiledModuleFileNode(
                    GetObjBuildPath(ifaceFile, compilerInfo.cppCompiledModuleExt),
                    CppCompiledModuleRecipe(compilerInfo, modules))
                AddNode(compiledModule)
            }
            module.implFiles.forEach {
                implFile ->
                val objectFile = ObjectFileNode(
                        GetObjBuildPath(implFile, compilerInfo.objFileExt),
                        CppObjectRecipe(compilerInfo, modules))
                AddNode(objectFile)
            }

            return resultNodes
        }
    }
}