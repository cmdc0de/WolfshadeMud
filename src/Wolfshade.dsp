# Microsoft Developer Studio Project File - Name="main" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=main - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Wolfshade.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wolfshade.mak" CFG="main - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "main - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "main - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "main - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "d:\mud\wolfshademud\src\release"
# PROP Intermediate_Dir "d:\mud\wolfshademud\src\release"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WOLFSHADE_WINDOWS_" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\wolfshade.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib shell32.lib ws2_32.lib wsock32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\debug"
# PROP Intermediate_Dir "..\debug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WOLFSHADE_WINDOWS_" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib shell32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386

!ENDIF 

# Begin Target

# Name "main - Win32 Release"
# Name "main - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\affect.cpp
# End Source File
# Begin Source File

SOURCE=.\affectedby.cpp
# End Source File
# Begin Source File

SOURCE=.\anti.paladin.cpp
# End Source File
# Begin Source File

SOURCE=.\armor.cpp
# End Source File
# Begin Source File

SOURCE=.\ascii.stream.cpp
# End Source File
# Begin Source File

SOURCE=.\attributes.cpp
# End Source File
# Begin Source File

SOURCE=.\bank.room.cpp
# End Source File
# Begin Source File

SOURCE=.\barbarian.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\brownie.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\centaur.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\char.intermediate.cpp
# End Source File
# Begin Source File

SOURCE=.\character.attributes.cpp
# End Source File
# Begin Source File

SOURCE=.\character.cpp
# End Source File
# Begin Source File

SOURCE=.\cleric.cpp
# End Source File
# Begin Source File

SOURCE=.\container.cpp
# End Source File
# Begin Source File

SOURCE=.\corpse.cpp
# End Source File
# Begin Source File

SOURCE=.\defiler.cpp
# End Source File
# Begin Source File

SOURCE=.\draconian.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\drow.elf.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\druid.cpp
# End Source File
# Begin Source File

SOURCE=.\duergar.dwarf.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\elemental.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\empath.cpp
# End Source File
# Begin Source File

SOURCE=.\error.cpp
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\githyanki.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\global.management.cpp
# End Source File
# Begin Source File

SOURCE=.\gnoll.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\gnome.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\grey.elf.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\group.cpp
# End Source File
# Begin Source File

SOURCE=.\half.drow.elf.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\half.grey.elf.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\half.orc.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\human.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\illithid.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\imp.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\interpetor.cpp
# End Source File
# Begin Source File

SOURCE=.\kender.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\kenku.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\lich.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\light.cpp
# End Source File
# Begin Source File

SOURCE=.\list.cpp
# End Source File
# Begin Source File

SOURCE=.\mage.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\message.manager.cpp
# End Source File
# Begin Source File

SOURCE=.\mindflayer.cpp
# End Source File
# Begin Source File

SOURCE=.\minotaur.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\mob.manager.cpp
# End Source File
# Begin Source File

SOURCE=.\mob.prototype.cpp
# End Source File
# Begin Source File

SOURCE=.\money.cpp
# End Source File
# Begin Source File

SOURCE=.\mountain.dwarf.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\mudtime.cpp
# End Source File
# Begin Source File

SOURCE=.\mul.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\none.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.antipaladin.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.classless.ai.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.cleric.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.defiler.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.druid.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.empath.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.mage.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.mindflayer.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.paladin.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.psionicist.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.ranger.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.rogue.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.shaman.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.warrior.cpp
# End Source File
# Begin Source File

SOURCE=.\object.cpp
# End Source File
# Begin Source File

SOURCE=.\object.manager.cpp
# End Source File
# Begin Source File

SOURCE=.\object.misc.cpp
# End Source File
# Begin Source File

SOURCE=.\object.prototype.cpp
# End Source File
# Begin Source File

SOURCE=.\object.save.cpp
# End Source File
# Begin Source File

SOURCE=.\ogre.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\orc.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\paladin.cpp
# End Source File
# Begin Source File

SOURCE=.\pixie.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\portal.cpp
# End Source File
# Begin Source File

SOURCE=.\psionicist.cpp
# End Source File
# Begin Source File

SOURCE=.\racial.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\ranger.cpp
# End Source File
# Begin Source File

SOURCE=.\rogue.cpp
# End Source File
# Begin Source File

SOURCE=.\room.cpp
# End Source File
# Begin Source File

SOURCE=.\saurial.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\save.char.cpp
# End Source File
# Begin Source File

SOURCE=.\shaman.cpp
# End Source File
# Begin Source File

SOURCE=.\shop.room.cpp
# End Source File
# Begin Source File

SOURCE=.\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\stdinclude.cpp
# End Source File
# Begin Source File

SOURCE=.\thrikreen.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\trackinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\transport.cpp
# End Source File
# Begin Source File

SOURCE=.\transportroom.cpp
# End Source File
# Begin Source File

SOURCE=.\troll.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\undead.reference.cpp
# End Source File
# Begin Source File

SOURCE=.\warrior.cpp
# End Source File
# Begin Source File

SOURCE=.\wolfshade.string.cpp
# End Source File
# Begin Source File

SOURCE=.\world.cpp
# End Source File
# Begin Source File

SOURCE=.\zone.cpp
# End Source File
# Begin Source File

SOURCE=.\zone.manager.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Affect.h
# End Source File
# Begin Source File

SOURCE=.\anti.paladin.h
# End Source File
# Begin Source File

SOURCE=.\armor.h
# End Source File
# Begin Source File

SOURCE=.\ascii.stream.h
# End Source File
# Begin Source File

SOURCE=.\attributes.h
# End Source File
# Begin Source File

SOURCE=.\bank.room.h
# End Source File
# Begin Source File

SOURCE=.\barbarian.reference.h
# End Source File
# Begin Source File

SOURCE=.\brownie.reference.h
# End Source File
# Begin Source File

SOURCE=.\caster.h
# End Source File
# Begin Source File

SOURCE=.\centaur.reference.h
# End Source File
# Begin Source File

SOURCE=.\char.intermediate.h
# End Source File
# Begin Source File

SOURCE=.\character.attributes.h
# End Source File
# Begin Source File

SOURCE=.\Character.h
# End Source File
# Begin Source File

SOURCE=.\Cleric.h
# End Source File
# Begin Source File

SOURCE=.\color.h
# End Source File
# Begin Source File

SOURCE=.\Constants.h
# End Source File
# Begin Source File

SOURCE=.\Container.h
# End Source File
# Begin Source File

SOURCE=.\corpse.h
# End Source File
# Begin Source File

SOURCE=.\Defiler.h
# End Source File
# Begin Source File

SOURCE=.\descriptor.h
# End Source File
# Begin Source File

SOURCE=.\draconian.reference.h
# End Source File
# Begin Source File

SOURCE=.\drow.elf.reference.h
# End Source File
# Begin Source File

SOURCE=.\Druid.h
# End Source File
# Begin Source File

SOURCE=.\duergar.dwarf.reference.h
# End Source File
# Begin Source File

SOURCE=.\elemental.reference.h
# End Source File
# Begin Source File

SOURCE=.\Empath.h
# End Source File
# Begin Source File

SOURCE=.\Error.h
# End Source File
# Begin Source File

SOURCE=.\Game.h
# End Source File
# Begin Source File

SOURCE=.\githyanki.reference.h
# End Source File
# Begin Source File

SOURCE=.\global.functions.h
# End Source File
# Begin Source File

SOURCE=.\global.management.h
# End Source File
# Begin Source File

SOURCE=.\global.race.reference.h
# End Source File
# Begin Source File

SOURCE=.\gnoll.reference.h
# End Source File
# Begin Source File

SOURCE=.\gnome.reference.h
# End Source File
# Begin Source File

SOURCE=.\grey.elf.reference.h
# End Source File
# Begin Source File

SOURCE=.\half.drow.elf.reference.h
# End Source File
# Begin Source File

SOURCE=.\half.grey.elf.reference.h
# End Source File
# Begin Source File

SOURCE=.\half.orc.reference.h
# End Source File
# Begin Source File

SOURCE=.\hash.h
# End Source File
# Begin Source File

SOURCE=.\human.reference.h
# End Source File
# Begin Source File

SOURCE=.\illithid.reference.h
# End Source File
# Begin Source File

SOURCE=.\imp.reference.h
# End Source File
# Begin Source File

SOURCE=.\interpetor.h
# End Source File
# Begin Source File

SOURCE=.\kender.reference.h
# End Source File
# Begin Source File

SOURCE=.\kenku.reference.h
# End Source File
# Begin Source File

SOURCE=.\lich.reference.h
# End Source File
# Begin Source File

SOURCE=.\light.h
# End Source File
# Begin Source File

SOURCE=.\LinkList.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\Mage.h
# End Source File
# Begin Source File

SOURCE=.\memory.allocator.h
# End Source File
# Begin Source File

SOURCE=.\message.manager.h
# End Source File
# Begin Source File

SOURCE=.\Mindflayer.h
# End Source File
# Begin Source File

SOURCE=.\minotaur.reference.h
# End Source File
# Begin Source File

SOURCE=.\mob.manager.h
# End Source File
# Begin Source File

SOURCE=.\mob.prototype.h
# End Source File
# Begin Source File

SOURCE=.\MobAI.h
# End Source File
# Begin Source File

SOURCE=.\money.h
# End Source File
# Begin Source File

SOURCE=.\mountain.dwarf.reference.h
# End Source File
# Begin Source File

SOURCE=.\mudtime.h
# End Source File
# Begin Source File

SOURCE=.\mul.reference.h
# End Source File
# Begin Source File

SOURCE=.\none.reference.h
# End Source File
# Begin Source File

SOURCE=.\npc.classes.h
# End Source File
# Begin Source File

SOURCE=.\NPC.h
# End Source File
# Begin Source File

SOURCE=.\Object.h
# End Source File
# Begin Source File

SOURCE=.\object.manager.h
# End Source File
# Begin Source File

SOURCE=.\object.misc.h
# End Source File
# Begin Source File

SOURCE=.\object.prototype.h
# End Source File
# Begin Source File

SOURCE=.\object.save.h
# End Source File
# Begin Source File

SOURCE=.\ogre.reference.h
# End Source File
# Begin Source File

SOURCE=.\orc.reference.h
# End Source File
# Begin Source File

SOURCE=.\Paladin.h
# End Source File
# Begin Source File

SOURCE=.\pixie.reference.h
# End Source File
# Begin Source File

SOURCE=.\portal.h
# End Source File
# Begin Source File

SOURCE=.\proficiency.h
# End Source File
# Begin Source File

SOURCE=.\Psionicist.h
# End Source File
# Begin Source File

SOURCE=.\racial.reference.h
# End Source File
# Begin Source File

SOURCE=.\random.h
# End Source File
# Begin Source File

SOURCE=.\Ranger.h
# End Source File
# Begin Source File

SOURCE=.\rogue.h
# End Source File
# Begin Source File

SOURCE=.\Room.h
# End Source File
# Begin Source File

SOURCE=.\saurial.reference.h
# End Source File
# Begin Source File

SOURCE=.\save.char.h
# End Source File
# Begin Source File

SOURCE=.\Shaman.h
# End Source File
# Begin Source File

SOURCE=.\shop.room.h
# End Source File
# Begin Source File

SOURCE=.\socket.h
# End Source File
# Begin Source File

SOURCE=.\Spell.h
# End Source File
# Begin Source File

SOURCE=.\spell.save.h
# End Source File
# Begin Source File

SOURCE=.\Stdinclude.h
# End Source File
# Begin Source File

SOURCE=.\template.functions.h
# End Source File
# Begin Source File

SOURCE=.\thinker.h
# End Source File
# Begin Source File

SOURCE=.\thrikreen.reference.h
# End Source File
# Begin Source File

SOURCE=.\trackinfo.h
# End Source File
# Begin Source File

SOURCE=.\transport.h
# End Source File
# Begin Source File

SOURCE=.\transportroom.h
# End Source File
# Begin Source File

SOURCE=.\troll.reference.h
# End Source File
# Begin Source File

SOURCE=.\undead.reference.h
# End Source File
# Begin Source File

SOURCE=.\wall.h
# End Source File
# Begin Source File

SOURCE=.\warrior.h
# End Source File
# Begin Source File

SOURCE=.\wolfshade.string.h
# End Source File
# Begin Source File

SOURCE=.\World.h
# End Source File
# Begin Source File

SOURCE=.\Zone.h
# End Source File
# Begin Source File

SOURCE=.\zone.manager.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\zone.helpers.inc
# End Source File
# End Target
# End Project
