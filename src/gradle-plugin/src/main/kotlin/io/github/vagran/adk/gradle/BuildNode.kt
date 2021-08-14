package io.github.vagran.adk.gradle

import org.gradle.api.Task

open class BuildNode(val recipe: Recipe?) {
    val dependencies = ArrayList<BuildNode>()
    var task: Task? = null
}