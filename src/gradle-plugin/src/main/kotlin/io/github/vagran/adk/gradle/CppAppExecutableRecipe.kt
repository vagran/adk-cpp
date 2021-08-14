package io.github.vagran.adk.gradle

/** @param modules All modules of the executable (with all direct and indirect dependencies) */
class CppAppExecutableRecipe(private val compilerInfo: CompilerInfo,
                             private val modules: List<ModuleNode>): Recipe {

    override fun CreateTask(artifact: BuildNode)
    {
        /* Resulted binary has only direct dependencies, so need to traverse all dependant nodes. */
        TODO("Not yet implemented")
    }

    override val name = "Link C++ app executable"
}