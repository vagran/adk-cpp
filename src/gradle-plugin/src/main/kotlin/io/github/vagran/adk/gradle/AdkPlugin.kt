package io.github.vagran.adk.gradle

import org.gradle.api.Plugin
import org.gradle.api.Project

class AdkPlugin: Plugin<Project> {

    override fun apply(project: Project)
    {
        val extension: GreetingExtension =
                project.extensions.create("greeting", GreetingExtension::class.java)

        val adkExt: AdkExtension =
            project.extensions.create("adk", AdkExtension::class.java, project)

        val task = project.tasks.register("greeting", GreetingTask::class.java)
        project.afterEvaluate {
            p: Project? ->
            task.configure {
                t: GreetingTask ->
                t.who = extension.who
            }
        }
    }
}
