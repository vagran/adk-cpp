package io.github.vagran.adk.gradle

/** @param modules Target module with all direct and indirect dependencies. */
class CppCompiledModuleRecipe(private val compilerInfo: CompilerInfo,
                              private val modules: List<ModuleNode>): Recipe {
    override fun CreateTask(artifact: BuildNode)
    {
        TODO("Not yet implemented")
    }

    override val name = "Compile C++ module"
}