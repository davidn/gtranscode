<?xml version="1.0"?>
<anjuta>
    <plugin name="Macro Plugin" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-macro:MacroPlugin"/>
    </plugin>
    <plugin name="Patch Plugin" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-patch:PatchPlugin"/>
    </plugin>
    <plugin name="Tools" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-tools:ATPPlugin"/>
    </plugin>
    <plugin name="Valgrind Plugin" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-valgrind:AnjutaValgrindPlugin"/>
    </plugin>
    <plugin name="Profiler" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-profiler:Profiler"/>
    </plugin>
    <plugin name="Debugger" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-debug-manager:DebugManagerPlugin"/>
    </plugin>
    <plugin name="API Help" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-devhelp:AnjutaDevhelp"/>
    </plugin>
    <plugin name="Terminal" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-terminal:TerminalPlugin"/>
    </plugin>
</anjuta>
