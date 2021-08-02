package io.github.vagran.adk.gradle

import org.gradle.api.DefaultTask
import org.gradle.api.tasks.Input
import org.gradle.api.tasks.TaskAction


open class GreetingTask: DefaultTask() {
    @get:Input
    var who = "mate"

    @TaskAction
    fun greet() {
        println("Hi $who!!!")
    }
}