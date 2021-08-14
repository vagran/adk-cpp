package io.github.vagran.adk.gradle

class CppObjectRecipe(private val compilerInfo: CompilerInfo,
                      private val modules: List<ModuleNode>): Recipe {

    override fun CreateTask(artifact: BuildNode)
    {
        TODO("Not yet implemented")
    }

    override val name = "Compile C++ object"
}