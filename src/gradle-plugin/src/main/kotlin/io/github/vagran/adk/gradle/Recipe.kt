package io.github.vagran.adk.gradle

interface Recipe {
    fun CreateTask(artifact: BuildNode)

    val name: String
}