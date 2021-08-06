package io.github.vagran.adk.gradle

import groovy.lang.Closure
import org.gradle.api.Project
import java.io.File
import kotlin.reflect.KMutableProperty1
import kotlin.reflect.KProperty

open class ModuleExtension(private val project: Project, private val nested: Boolean) {

    fun CreateContext(baseDir: File): ModuleExtensionContext
    {
        return ModuleExtensionContext(project, baseDir).also { _ctx = it }
    }

    fun CloseContext()
    {
        _ctx = null
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////

    var name: String by ContextProp(ModuleExtensionContext::name)

    var include: List<File> by ContextProp(ModuleExtensionContext::include)

    fun include(vararg items: Any)
    {
        ctx.includeProp.Append(items)
    }

    var libDir: List<File> by ContextProp(ModuleExtensionContext::libDir)

    fun libDir(vararg items: Any)
    {
        ctx.libDirProp.Append(items)
    }

    var submodules: List<File> by ContextProp(ModuleExtensionContext::submodules)

    fun submodules(vararg items: Any)
    {
        ctx.submodulesProp.Append(items)
    }

    var impl: List<File> by ContextProp(ModuleExtensionContext::impl)

    fun impl(vararg items: Any)
    {
        ctx.implProp.Append(items)
    }

    var moduleMap: List<File> by ContextProp(ModuleExtensionContext::moduleMap)

    fun moduleMap(vararg items: Any)
    {
        ctx.moduleMapProp.Append(items)
    }

    var define: List<String> by ContextProp(ModuleExtensionContext::define)

    fun define(vararg items: String)
    {
        ctx.defineProp.Append(items)
    }

    var cflags: List<String> by ContextProp(ModuleExtensionContext::cflags)

    fun cflags(vararg items: String)
    {
        ctx.cflagsProp.Append(items)
    }

    var libs: List<String> by ContextProp(ModuleExtensionContext::libs)

    fun libs(vararg items: String)
    {
        ctx.libsProp.Append(items)
    }

    fun module(name: String, config: Closure<ModuleExtension>) {
        if (nested) {
            throw Error("Module block can be nested in top-level module block only")
        }
        val e = ModuleExtension(project, true)
        ctx.childContexts.add(e.CreateContext(ctx.baseDir))
        e.name = name
        project.configure(e, config)
        e.CloseContext()
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////

    private var _ctx: ModuleExtensionContext? = null

    private val ctx: ModuleExtensionContext get()
    {
        return _ctx ?: throw Error("Module extension members can be accessed in module script only")
    }

    private inner class ContextProp<V>(private val prop: KMutableProperty1<ModuleExtensionContext, V>) {

        operator fun getValue(thisRef: Any, property: KProperty<*>) = prop.get(ctx)

        operator fun setValue(thisRef: Any, property: KProperty<*>, value: V) = prop.set(ctx, value)
    }
}