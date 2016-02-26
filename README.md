# GifImageSource
GifImageSource is a Windows Runtime Component for Windows and Windows Phone 8, 8.1, and Universal Windows Projects, made for rendering GIF images in a resource efficient way using Direct2D.

Available on [NuGet](https://www.nuget.org/packages/GifImageSource/).

**Dependencies:**
This project depends on the [Microsoft Visual C++ Runtime SDK](http://i.imgur.com/0vddrk0.png). This dependency will be added automatically when installing from NuGet, but a reload of the project will be needed before use. If the app crashes when using this library, make sure the dependency is added.

**Basic usage from xaml:**
```xml
xmlns:gif="using:GifImage"
```
```xml
<Image gif:AnimationBehavior.ImageUriSource="http://someplace.com/MyGif.gif" />
```
```xml
<Image gif:AnimationBehavior.ImageUriSource="/Assets/MyGif.gif" />
```
**Basic usage from code:**

Uri:
```csharp
GifImage.AnimationBehavior.SetImageUriSource(MyImageControl, new Uri("ms-appx:///Assets/MyGif.gif"));
```
Stream:
```csharp
var storageFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/MyGif.gif"));
GifImage.AnimationBehavior.SetImageStreamSource(MyImageControl, await storageFile.OpenReadAsync());
```
**Duration controls:**

The play time or play count can be controlled via the RepeatBehavior property:
```xml
<Image gif:AnimationBehavior.RepeatBehavior="2x" /> //Repeats the animation twice
```
```xml
<Image gif:AnimationBehavior.RepeatBehavior="0:0:10" /> //Play for 10 seconds
```
or from code:
```csharp
GifImage.AnimationBehavior.SetRepeatBehavior(MyImageControl, new RepeatBehavior(5)); //repeat 5 times
```
```csharp
GifImage.AnimationBehavior.SetRepeatBehavior(MyImageControl, new RepeatBehavior(TimeSpan.FromSeconds(10))); //play for 10 seconds
```
If the repeatbehavior is not set, the GIFs own repeat count will be used when available.

**Playback controls:**

By default, GIFs autoplay when loaded. To disable autoplay, use the AutoStart property of AnimationBehavior:
```xml
<Image gif:AnimationBehavior.AutoStart="False" />
```
To start the animation:
```csharp
GifImageSource source = AnimationBehavior.GetGifImageSource(MyImageControl);
if (source != null)
  source.Start();
```
To pause the animation: 
```csharp
GifImageSource source = AnimationBehavior.GetGifImageSource(MyImageControl);
if (source != null)
  source.Pause();
```
To stop the animation and return to the first frame: 
```csharp
GifImageSource source = AnimationBehavior.GetGifImageSource(MyImageControl);
if (source != null)
  source.Stop();
```
How it works:
--------
Having a need for a GIF rendering library for a project, i tried out existing open source libraries out there. Each had strengths and weaknesses. 
Some had good cpu performance but used too much memory, which lead to crashes on mobile. Some did not follow all GIF specifications and rendered GIFs wrong, and some demanded too much of the CPU which lead to choppy playback on mobile hardware.
I deciced to create this project to combine the strenghts into one library, capable of efficiently rendering everything from the smallest 5kb emoticon, to high resolution GIFs with sizes in the dozens or even hundreds of MB.

The library is written as a Windows Runtime Component in C++, and will work on all Windows Runtime and UWP projects.

###Memory and CPU usage

A GIF is not necessarily a series of independent frames drawn one after another, but a series of images drawn upon each other, each one adding to the complete picture. 
A common strategy to make this work is to read all frames into memory, and then draw all frames that need to be rendered in series for each frame. 
For small GIFs this is a cpu efficient way to do it, but a problem occurs when you load a large GIF. Depending on color depth and resolution, a 10MB GIF can use 100MB of device memory or more.
Loading large GIFs could easily exceed the apps memory limit and crash the app.

The CPU usage can also steadily climb the longer the GIF plays, since it has to draw more and more bitmaps for each frame. A GIF can have hundreds of frames, so after 200 frames you could end up drawing 200 bitmaps every 20 to 40 milliseconds!

**But what is the alternative?**

The alternative is to read frames from the stream in real time, drawing on top of the previous frame each time. This solution uses more cpu due to the constant need to read data from the stream, but uses no more memory then the current frame, and the CPU usage is always more or less constant.
A problem with this solution is that it is susceptible to slowdowns or stuttering if the system can't read the frames fast enough.

This library uses a tweaked combination of the above methods. It caches frames up to a limit of 50 frames, and then starts a task that buffers 5 frames ahead of the current frame. This makes it as efficient as possible on small GIFs, while also supporting smooth playback of very large GIFs.

In case of a phone app, each GIF will periodically check app memory usage and stop caching frames when the memory usage exceeds a limit of 60% of app allowance, and clear all cached frames when it exceeds 80%. The library is tested to be able to smoothly render a 70MB GIF on the hardware of a Lumia 640 phone, and should work on all Windows Phone models.

**Gif specification:**

This library fully supports the GIF87A and GIF89A format, and the [Netscape Application Block](http://www.vurdalakov.net/misc/gif/netscape-looping-application-extension).
