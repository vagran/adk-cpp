package io.github.vagran.adk.gradle

import org.gradle.api.Task
import org.gradle.api.tasks.Exec
import java.io.File

/** @param modules All modules of the executable (with all direct and indirect dependencies) */
class CppAppExecutableRecipe(private val compilerInfo: CompilerInfo,
                             private val modules: List<ModuleNode>): Recipe {

    override fun CreateTask(artifact: BuildNode, taskFactory: Recipe.TaskFactory<*>): Task
    {
        /* Resulted binary has only direct dependencies, so need to traverse all dependant nodes. */
        artifact as ExecutableFileNode
        val task = taskFactory.CreateTask<Exec>()
        val cmd = compilerInfo.GetCommandLineBuilder()

        cmd.SetAction(CommandLineBuilder.Action.LINK)

        val sources = ArrayList<File>()
        sources.addAll(artifact.FindDepDeep<CppCompiledModuleFileNode>().map { it.path })
        sources.addAll(artifact.FindDepDeep<ObjectFileNode>().map { it.path })
        sources.forEach { cmd.AddInput(it) }

        cmd.SetOutput(artifact.path)

        val depModules = artifact.FindDep<CppCompiledModuleFileNode>()
        depModules.map { it.path }.forEach { cmd.AddModuleDep(it) }

        compilerInfo.adkConfig.libDir.forEach { cmd.AddLibDir(it) }
        modules.flatMap { it.libDir }.forEach { cmd.AddLibDir(it) }

        compilerInfo.adkConfig.libs.forEach { cmd.AddLib(it) }
        modules.flatMap { it.libs }.forEach { cmd.AddLib(it) }

        task.commandLine = cmd.Build().toList()
        task.inputs.files(sources)
        task.outputs.file(artifact.path)
        task.setDependsOn(depModules.map { it.task })
        return task
    }

    override val name = "Link C++ app executable"
    override val taskNamePrefix = "cpp_link_"
}