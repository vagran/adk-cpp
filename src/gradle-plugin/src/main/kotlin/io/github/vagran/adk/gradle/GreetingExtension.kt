package io.github.vagran.adk.gradle

open class GreetingExtension {
    var who: String = ""

    fun AddModule(name: String)
    {
        println("Module added: $name")
    }
}