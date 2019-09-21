GAME WALKTHROUGH:

A beginning walkthrough for the game is available.
Simply switch to your CD drive
and type "WALKTHRU".  This will show you a
game walkthrough.  To print this file, switch
to your CD drive and type "PRINT WALKTHRU".

WINDOWS USERS:

We highly recommend that you turn your screen savers
OFF while playing King's Quest VI.  If you are in
the middle of a conversation with a character
and your screen saver kicks in, you may
experience audio break ups, and other problems.

GUIDEBOOK TO THE LAND OF THE GREEN ISLES:

You may be tempted to (at some point) throw out the
Guidebook.  This is a warning not to, however.  This
guide contains detailed instructions to get past
certain points in the game.  You will not be able
to pass these points without the guide.

TANDY 2500 (and some other models):

If you notice a tremendous slowdown on a Tandy machine,
it may be due to a lack of memory.  There have been
noted problems where the Tandy may report more memory
than it actually has.  The solution to this is to free
up more memory, or make a boot disk.

NEW USERS:

If you are unfamiliar with Sierra's icon interface,
you may wish to select the HELP button from the
King's Quest VI title screen.

DEMOS DEMOS DEMOS!!!

There are quite a few demos on this CD.
They are all in the DEMOS subdirectory
on the game CD.

SPEECH BREAKUP QUESTIONS:

If you experience audio cutouts during the game you may need to 
alter the audio buffer size of the MSCDEX driver located in your 
AUTOEXEC.BAT file.  You can do this by adjusting the 
/m:AUDIOSIZE parameter.  We recommend that this value be set to 
at least 10.  If you experience slow movement or audio cutouts 
consistently, you may wish to lower the size of this buffer.  For more 
tips, please refer to the "OTHER NOTES" section.

KING'S QUEST VI DOS QUESTIONS:

Q: "Where's the SETUP floppy disk mentioned in the documentation
on page 5?"
A: There is no need for a KQ6 setup floppy disk.  Everything you need is 
included on the CD-ROM disc.

Q: "I start KQ6 and get a message that says I need more memory,
but I've got over 100 megabytes free on my hard drive.  What's wrong?"
A: Memory is not the same as hard drive space, although both are
measured in bytes.  Deleting files from the hard drive will NOT free 
more memory.  Try making a Sierra boot disk to free up the needed 
memory.

Q: "I start KQ6 and get a message that says I need more memory,
but I've got 8 megabytes of memory.  What's wrong?"
A: No matter how much memory you have installed, DOS is
designed to use only the first 640K of RAM.  Make and use a boot
disk.  The only way we can use all the memory in your computer is
to run KQ6 as a Windows application.  To play King's Quest VI, we
require that you have 589,000 bytes of available base memory. 

Q: "I start KQ6 and get a message that says I need 30,000 more
bytes of memory.  I run the CHKDSK command, and have 540,000 bytes 
free.  What do I do?"
A:  Make and use a Sierra boot disk.  If you still can't get enough
free memory, you may be running an earlier version of DOS.  KQ6
requires DOS 5.0 or higher to run.  To see what version of DOS 
you're using, type "VER" at the C:\> prompt. 

Q: "I occassionally get the error 'Out of Handles' when playing 
King's Quest VI.  What is that?"
A: On machines with a high amount of RAM (over 8 meg), you may 
occasionally experience an "Out of Handles" error.  There is 
currently no way to fix this. There are two solutions.  You may 
install to NOT use extended memory, or just restart the game when 
the error occurs.  We apologize for any inconvenience this may 
cause.

Q: "What's a boot disk?"
A: A boot disk is a formatted, bootable high density floppy disk in 
your A: drive, with a special CONFIG.SYS and AUTOEXEC.BAT,
designed to run KQ6 with no problem.  You still run KQ6 from your 
hard drive; you just start your computer with this boot disk in the A:
drive, then run KQ6 from the hard drive. You cannot use a boot disk
in the B: drive.
 
Q: "How do I make and use a Sierra boot disk?"
A: The easiest way is to run INSTALL and choose "MAKE A 
BOOTABLE FLOPPY DISK".  We do have specialized boot disk
instructions to mail or fax for customers running disk compression 
utility programs, such as STACKER or SUPERSTOR.  A "plain"
boot disk from another game may not work if it doesn't load your 
CD-ROM driver.

Q: "I don't have a sound card.  Why can't KQ6 use the CD-AUDIO
out port like some other of my CD-ROM software?"
A: A regular audio CD can hold only about an hour's worth of music 
and speech.  KQ6 has several hours worth of audio that must be
compressed to fit on the CD.  A sound card with a DAC chip, such 
as a Sound Blaster, is required to decompress and play back the audio files.

Q: "My sound card is not supported.  What do I do?"
A: If your sound card has a Windows driver, run KQ6 as a Windows
application.

Q: "I've got a Sound Blaster and I'm getting an error message
UNABLE TO INITIALIZE YOUR AUDIO HARDWARE.  What's going on?"
A: Your Sound Blaster is probably set to I/O address 240 rather
than the factory setting of 220.  You'll need to add this line
anywhere in the RESOURCE.CFG file in the C:\SIERRA\KQ6CD subdirectory:
AudioPort = 240
Some cards may be set to 210, 220, 230, 240, 250, or 260.
The Sound Blaster or compatible card must run at DMA 1.

Q:  "I have a Pro Audio Spectrum 16/Plus and I am having 
problems."
A:  If you are using a Pro AudioSpectrum 16 or Plus card and 
experience problems during the opening cartoon or during the game, 
rerun the INSTALL utility and select "Sound Blaster" as your audio 
device.  Selecting "Sound Blaster" will utilize the PAS card's built-
in Sound Blaster emulation for digitized audio effects.

Q: "I've got a Sound Blaster Pro sound card.  The voices are fine,
but I can't hear any music.  What's wrong?"
A: The Sound Blaster Pro comes with mixer software.  The
SYNTHESIZER channel is probably much lower than the DAC channel.  From the 
C:\SBPRO> prompt, type "SBP-MIX", then press ALT 1 to adjust the volume 
levels.  After this, type "SBP-MIX -U" to remove the mixer software
from memory so KQ6 will have enough memory to run.

Q: "I've got a Proaudio Spectrum sound card.  The voices are fine,
but I can't hear any music.  What's wrong?"
A: The Proaudio Spectrum cards come with mixer software.  The 
SYNTHESIZER channel is probably much lower than the DAC channel.  From the 
C:\PROAUDIO> prompt, type "PAS *" to adjust the volume levels.

Q: "I've got a Gravis Ultrasound card.  How can I make it work?"
A: The Gravis Ultrasound card comes with software to allow it to 
emulate a Sound Blaster sound card.  Run the "SBOS -X2" command
to make the GUS work with KQ6.

Q: "The music is fine, but the voices hesitate or skip.  What can I
do?"
A: Increase the audio buffer size for smoother speech.  First, make 
sure you're using a boot disk to free up plenty of memory.  Edit the 
RESOURCE.CFG file in the C:\SIERRA\KQ6CD> directory.  The default is 
"AudioSize = 8K".  Increase the size to "24K" for smoother speech with
more free memory.  Note that this is for KQ6 running under DOS 
(rather than Windows.)

Q: "I don't have a mouse.  How do I move the hand cursor in smaller
increments?"
A: Hold down the SHIFT key when using the cursor keys to move
the pointer in smaller increments.

Q: "My mouse won't work.  Help!"
A: The mouse driver must be loaded before the game can use the
mouse.  If you're unfamiliar with this, the boot disk maker will
try to find and load MOUSE.COM or MOUSE.SYS for you from the hard 
drive.  The most confusing thing may be that the mouse works in
Windows, but not in DOS.  If the mouse is not loaded through
DOS, Windows will use its own mouse driver.  When you exit or 
shell out of Windows, this mouse driver is unloaded from memory. 
Try a boot disk to enable the mouse in DOS.

Q:  "How do I run the game directly from my CD?"
A:  Because King's Quest VI is a very machine specific game, we 
require that you install it to a hard drive.  This will enable 
the game to utilize your unique configuration.


KING'S QUEST VI WINDOWS QUESTIONS:

Q: "Where's the SETUP floppy disk mentioned in the documentation
on page 5?"
A: There is no need for a setup floppy disk.  Everything you need is 
included on the CD-ROM disc.

Q:  "I have never installed a Sierra game through Windows.  What 
do I do?"
A:  To install King's Quest VI through Windows, you need to do the 
following:

1)  Enter Windows.

2)  From the FILE menu, select RUN.

3)  Type d:\setup (or whatever your CD drive designation is).

4)  Follow the prompts.

This will allow you to play the game through Windows.  It
will create a SIERRA Program Group, and King's Quest VI
will be an item in that group.

Q: "What resolutions does the game run in?"
A: The following display modes are supported in Windows:
1024 x 768 in 256 color
800 x 600 in 256 color
640 x 480 in 256 color

Q: "I'm running in 1024 by 768 mode.  How do I increase the KQ6 
window size to fill out the entire screen?"
A: KQ6 will fill the entire screen in 640 x 480 mode.  The higher
the display resolution, the smaller the KQ6 window.  You cannot 
"blow up" the KQ6 window to fill the screen in 800 x 600 or in 
1024 x 768, since it runs at a fixed resolution of 640 by 
480.  You should be able to run the game in DOS to fill up the 
entire screen.

Q: "I've got a Super VGA video card.  Why does KQ6 say I'm not in 256
color mode?"
A: You may not be running in 256 color display mode.  Check this by
going to the MAIN program group, then WINDOWS SETUP, to see your 
current display mode. "VGA" indicates that you're running in 640 x 480
in 16 color.  To run in Super VGA's 256 color mode, the Super VGA
display driver would need to be loaded.  These Super VGA drivers are
provided by the video card company rather than by Microsoft, so they 
don't come standard with Windows.  If you're unfamiliar with
installing these drivers, consult your video card manual or your
dealer for more information, or talk to the person who set up 
Windows for you.

Q: "I can't run Windows in 256 color mode.  How do I run the game?"
A: Run KQ6 under DOS.

Q: "KQ6 doesn't run; I get a black window instead.  What's wrong?"
A: You may be running in 32000 or higher color mode.  KQ6 runs
only in 256 color display mode.  

Q: "KQ6 runs okay, but it looks a little fuzzy.  Is anything wrong?"
A: No.  KQ6 was originally designed to run in 320 x 200 in 256
color mode.  If you like, try running the game under DOS to see
if it looks crisper.  Under Windows, some display modes will look
crisper than other display modes when running KQ6.

Q: "KQ6 looks weird running on my ATI Ultra Pro SVGA card.  
What can I do?"
A: The symptoms may be jigsaw-like screens or black backgrounds.
First make sure you are using the latest ATI Windows SVGA driver.  
If you're still having problems with the latest ATI video driver and you 
want to temporarily force KQ6 to work, there are two ways:
1: In Windows, go into the MACH32 Utilities program group, click
FlexDesk Control Panel, then turn the bitmapping off and turn on 
the palette rotation.  (If you don't have FlexDesk loaded, follow step 2) 
2: Make a backup copy of WIN.INI, then edit the \WINDOWS\ WIN.INI file.  
Look for the [MACH 32] section.  To make KQ6 work, these specific lines 
should read as follows:
DEVICEBITDRAW=OFF
DEVICEBITMAPDRAW=OFF
PALETTEMANAGER=ON

Q: "What's the difference between BASE and EXTENDED sound
cards?"
A: The basic difference between the two is with the number of
instruments and notes that each can play.  BASE includes the
Sound Blaster, Thunderboard, and Proaudio Spectrum cards.
BASE level uses channels 13 through 16 in the MIDI Mapper.
EXTENDED includes the Roland MT-32, CM-32, and LAPC-I,
and uses channels 1 through 10 in the MIDI Mapper.
If you have a BASE card, choosing EXTENDED will not allow 
for better music.  

Q: "The music is fine but the voice volume is low, or vice-versa.  
What can I do?"
A: Try adjusting the VOLUME level from the control panel in KQ6.
Many sound cards also come with Windows mixer software that can
adjust the SYNTHESIZER and DAC volume levels separately.

Q: "The music is fine, but the voices hesitate or skip.  What can I 
do?"
A: Increase the audio buffers for smoother speech.  Edit the
SIERRA.INI file in the C:\SIERRA\KQ6CD\WIN> directory.  The default is
"AudioBuffs = 2".  Increase the size to 3 (or higher) for smoother speech.
Note that this is for KQ6 running under Windows (rather than DOS.)

Q: "I've got a Roland MT-32 (or a Sound Canvas, or a General 
MIDI device) and a Sound Blaster.  The speech is okay, but the music
and sound effects are incorrect.  What can I do?"
A: Quit the game, reset the EXTENDED sound device, then restart the
game.  This will flush the odd sounds that may have been loaded by the
previous game. 

Q: "KQ6 runs slow under Windows, even with the SPEED control at
the fastest setting.  What can I do?"
A: If SPEED is at maximum and DETAIL is at minimum and the game
doesn't run as fast as you'd like, run the game under DOS. 
Windows adds a lot of overhead that demands a faster computer
(a 386-40 or faster) to run KQ6 faster.  You should find 
the game will work faster under DOS.  You may also choose
to play in the small window mode for Windows.

Q: "I get to the end of the game, and I hear the closing song.
But, it's not accessing my CD drive, and the song doesn't sound
as good as a CD that I would buy at the store.  What's wrong?"
A: Try going into your control panel and adding the [MCI]CDAUDIO
driver under your drivers menu.  If this doesn't work, just 
put the CD into your home CD player and select song 2.

OTHER NOTES:
If you have random problems, make sure that you are using the
latest Microsoft CD-ROM extensions driver, version 2.21.  Contact 
Microsoft or your local dealer if you need the latest Microsoft
CD-ROM driver.

If you have any random problems while using the mouse playing 
from DOS, try using a different or newer mouse driver (preferably 
a Microsoft or Logitech mouse driver.)

If you have any random problems running under Windows, you may
be running Windows enhancement software, such as HP's Dashtop, Norton
Desktop, or Kid Desk. Please refer to their documentation for 
troubleshooting tips.

You cannot use saved games from the Windows when playing under
DOS, and vice-versa.  If you try, you'll get a message "THAT GAME 
WAS SAVED UNDER A DIFFERENT INTERPRETER."  Saved games
from a small installation cannot be used with a large installation, and
vice versa.

If the audio still skips, one of these suggestions should help (after
you have KQ6's INSTALL program make a boot disk for you):
1: Increase the audio buffer size in RESOURCE.CFG (under DOS) or 
in SIERRA.INI (under Windows.)  
2: On some CD-ROM drives, such as the Mitsumi and Tandy CD-ROM drives, there 
is a /M:X stock buffers parameter for the CD-ROM device driver line in 
CONFIG.SYS.  Adding /M:20 should eliminate audio skipping and choppy
cartoons.  Example of CONFIG.SYS line:
DEVICEHIGH=C:\DEV\MTMCDD.SYS /D:MSCD001 /P:390 /I:5 /M:20  
3: Increase the DMA buffers line on the EMM386 line in the CONFIG.SYS.
EXAMPLE: DEVICE=C:\WINDOWS\EMM386.EXE RAM D=64
4: You may need to increase the audio buffer size of the MSCDEX driver 
in the AUTOEXEC.BAT file on your boot disk.  The default size is "/M:2". 
If you have over 600K of free memory, try increasing the size to "/M:10".
If you experience slow movement or audio cutouts consistently, you may
wish to lower the size of this buffer.  

The VOLUME level control in KQ6 is for the music only, not the speech.

The final song, "Girl in the Tower" will play through the CD-ROM Audio Port
if your sound card cannot play 44Khz samples, such as the Sound Blaster or 
Thunderboard. 

As with other Windows applications, installing King's Quest 6 will update
your SYSTEM.INI and WIN.INI files.  If you want to immediately restore
your system environment after installing KQ6 CD-ROM, we took the added 
precaution of backing up your original WIN.INI and SYSTEM.INI files 
into the \SIERRA\KQ6CD directory.  The WIN.INI file is renamed KQ6.WIN, 
and SYSTEM.INI is renamed KQ6.INI.  Normally there is no need to restore
these files, since these files are constantly updated every time you use
Windows.  Do not copy and rename these files to the WINDOWS directory a 
few days, weeks, etc. after installing KQ6, or you'll lose the changes 
made after installing KQ6.  

MORE INFORMATION
If you continue to experience any problems, or if you have any questions
concerning any of the above steps, our Technical Support Team will be
more than happy to assist you.  Please call (209) 683-8989 between 8:15 a.m.
and 4:45 p.m., Pacific Standard Time, Monday through Friday,   We can be
reached by fax at (209) 683-3633, or by mail at the following address:

Sierra On-Line
P.O. Box 800
Coarsegold CA 93614-0800
Attention: Technical Support

You can also reach our Technical Support Team on one of the following
services:

Sierra BBS     (209) 683-4463  (settings 8, N, & 1, up to 38,000 baud)  
Compuserve     GAMEPUB (Technical Support ID 76004,2143)
PRODIGY        (Technical Support ID WBWW55A)
GEnie
America's Online

Please outline the problems along with the specific information about your
computer system, and we will gladly respond to your letter, fax, or BBS 
message as soon as possible.

To better serve our European customers with technical problems and disk 
replacements, Sierra U.K Customer Support can be reached at [0] 734 303171,
or by mail at the following address:

Sierra On-Line Limited
Unit 2, Theale Technology Centre, 
Station Road
Theale, Berkshire RG7 4AA
United Kingdom


SALES ORDER INFORMATION

To place a domestic order with Sierra On-Line, call (800) 326-6654 from
7:00 a.m. through 7:00 p.m., Pacific Standard Time, Monday through Friday,
for convenient, person-to-person service.  If you prefer, you may place an
order via fax (209) 683-4297 (please include your name, address, phone 
number, credit card number and expiration date.)

To place an international order with our Sierra On-Line United Kingdom
office, please call [44]734-303171 for convenient, person-to-person sevice, 
or fax in your order to [44]734-303201.  You can also place your 
international order with our U.S. office at (209) 683-4468, or fax in 
your order to (209) 683-4297.


HINTS

Game hints are available by hintbooks (orders available through the 
Sierra Sales Department), through the Sierra Bulletin Board Service
(209-683-4463 in the U.S., and [44]734-304227 in the U.K.), by fax, by mail
(P.O. Box 200, Coarsegold CA 93616-0200 Attention Hint Department), or 
by our automated 900 touchtone phone line.  

If you want an immediate hint answer, you can call our 24 hour Hint Line 
at (900) 370-5583 in the U.S.  In the U.K., call [44]734-304004.  The
charge is 75 cents for each minute.  Long distance charges are included 
in the fee.  U.K. customers will be charged the normal rate for 
U.K. calls.  Callers under the age of 18 must get their parents'
permission before calling the hint line.  ABSOLUTELY NO HINTS WILL BE 
PROVIDED THROUGH OUR CUSTOMER SUPPORT LINES.  At this time, the 
automated hint system is only available within the United States. 
All contents are copyrighted by Sierra On-Line and may not be 
reproduced in any form without express written permission.

RECORDED ANSWERS TO FREQUENTLY ASKED TECHNICAL QUESTIONS

Answers to our most frequently asked technical questions are available by 
calling our Technical Support Line at (209) 683-8989 and pressing "2" on
your touchtone telephone.  If your specific issue is not addressed, an 
option to speak to a Technical Support Team representative will be 
available.  Answers to our most frequently asked technical questions are 
also available through our Sierra BBS.  At the main menu, select option 
"8" for TECHNICAL HELP, and then select "T" for TOP TEN TECHNICAL 
ISSUES.

THE SIERRA NO RISK GUARANTEE!

We will gladly replace your program free of charge for the first 90 days
of ownership (please enclose a copy of your dated sales receipt with your
request.)  After 90 days there is a $10.00 U.S. (or 6 pound U.K. currency)
charge.  Send the program to:

U.S.                    U.K.
Sierra On-Line          Sierra On-Line Limited
P.O. Box 485            Unit 2, Theale Technology Centre,
Coarsegold CA 93644     Station Road
Attention: Returns      Theale, Berkshire RG7 4AA
                        United Kingdom

THE PROMISE: We want you to be happy with every Sierra product you
purchase from us.  Period.  If for any reason you're unhappy with 
the product, return it within 30 days for an exchange or a full 
refund...EVEN IF YOU BOUGHT IT RETAIL!  (Hardware ordered 
directly must be returned within 10 days.)

THE ONLY CATCH:  You gotta tell us why you don't like it.   
Otherwise we'll never get better.  Send it back to us and 
we promise we'll make things right.  (If you bought it at 
a retail outlet, please send your original sales receipt.)







