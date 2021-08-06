package io.github.vagran.adk.gradle

import org.gradle.api.Plugin
import org.gradle.api.Project

class AdkPlugin: Plugin<Project> {

    override fun apply(project: Project)
    {
        val adkExt: AdkExtension =
            project.extensions.create("adk", AdkExtension::class.java, project)

        val moduleExt: ModuleExtension =
            project.extensions.create("module", ModuleExtension::class.java, project, null)

//        val task = project.tasks.register("greeting", GreetingTask::class.java)

        project.afterEvaluate {
//            task.configure {
//                t: GreetingTask ->
//                t.who = extension.who
//            }

            val registry = ModuleRegistry(adkExt)

            registry.Build {
                dirPath ->
                val scriptPath = dirPath.resolve("module.gradle")
                if (!scriptPath.exists()) {
                    return@Build null
                }
                if (!scriptPath.isFile) {
                    throw Error("module.gradle should be regular file: $scriptPath")
                }
                val ctx = moduleExt.CreateContext(dirPath)
                project.apply(mapOf("from" to scriptPath))
                moduleExt.CloseContext()
                return@Build ctx
            }
        }
    }
}
