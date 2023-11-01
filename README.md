 # TailMeasure
 
 <p align="center">
  <img src="Images/TailMeasureGUI.png"  width="300" height="120" alt="TailMeasure GUI"/>
</p>

 TailMeasure is a very simple and very specific tool for VST reverb effect developement. Its sole purpose is to measure the length of a reverb tail, which is important information for parameter tuning. The conventional measurement to do so is called RT60, the time that it takes for the reverb to decay to a level of -60 dB.
 
 If you are currently implementing decay time parameter in your own reverb plugin, there may be some helpful information further down under "Implementing the Decay Time Parameter in Plugin Developement".

 ## Building TailMeasure

 To build it I recommend using the Projucer file. For this, you need to have JUCE set up. Alternatively, I tried to provide the VST3 plugin file directly in the eponymous folder. For some reason, GitHub displays this as a directory. Regardless, after cloning I believe it should be possible to simply drag it into your plugin folder.
 
 ## Setting Up TailMeasure
 
 TailMeasure really is meant for use within JUCE's AudioPluginHost. Set it up by feeding the inputs and outputs of TailMeasure and the reverb into each other in a figure-8 manner like this:
  
 <p align="center">
  <img src="Images/IOConfiguration.png" alt="IO Configuration"/>
</p>
 
 TailMeasure does not let any audio pass through, so there is no danger of feedback.
 
 Also note that TailMeasure works with white noise and therefore with the whole frequency band. Some definitions of RT60 state that only the levels in the range of human speech are used for measurement (~ 1 - 3 kHz). If this is what you're going for, you will need to include the necessary filters in the loop, behind the outputs of TailMeasure and before the input of the reverb.
  
 A testing environment such as AudioPluginHost may be necessary. I don't think it will work in a "conventional" DAW such as Cubase or Logic, because of the way you need to have TailMeasure and the reverb VST feed into each other. To my knowledge, this isn't possible in a classic DAW. However, you could use a completely different sound source and time the ending of said sound to coincide with the start of the measurement.
 
 ## Usage
 
 Once you have set it up, use it by simply pressing "Start Test". This will broadcast white noise on both channels into the reverb for three seconds, allowing it ample time to build up. Immediately afterwards, it measures the initial amplitude of the reverb tail, followed by continuous amplitude measurements of the decaying tail. As soon as the threshold of -60 dB has been reached, the result will be displayed on screen.
 
 Note that you will not hear any sound during the testing process as the sound samples are only exchanged between the two plugins.
 
 A mix parameter should have no influence on the outcome, as this has no influence on the relative volumes. The same goes for predelay (as long as it is below 3 seconds).
 
  ## Implementing the Decay Time Parameter in Plugin Developement
  
    I made TailMeasure as a tool to assist in the developement of a reverb plugin - specifically for implementing (you guessed it) the decay time parameter. 
    
    Depending on the reverb architecture, it can be a problem if you have more than one user parameter that does anything with any of the internal delays of the reverb. This is because altering any such parameter will also alter the decay time. Therefore, editing each such parameters requires an adaption of the delay times responsible for the decay time.
  
    My first (and folly) approach to implementing the decay time user parameter was therefore the use of internal scaling parameters that would scale the decay delay times as a reaction to other user parameter changes. The process  would look something like this:

    <ul>
        <li>Measure the decay time with TailMeasure at setting A</li>
        <li>Measure the decay time with TailMeasure at setting B</li>
        <li>Interpolate and determine internal scaling parameters</li>
        <li>???</li>
        <li>Done.</li>
    </ul>

    This is of course grossly simplified, but if your approach was anything along those lines, I strongly would advise to not make the same mistake I made. It worked  reasonably well for adapting a single user parameter (such as size), but if you have more than one then they interact with each other in an unpredictable way.

    I have found (and maybe this should have occurred to me sooner) that the right way of doing this was to make a formula that sets decay times and gain in relationship to one another. The exact solution depends very much on the architecture of the algorithm, but I will give an example of how it worked for me:

    Say you have a signal loop that contains a gain (with gain "g") block and two delay blocks. Let's say that the delay block D1 is tied to some unrelated user parameter such as "Size". The other block D2 is then solely responsible for ensuring the correct reverb time. For each time the signal revolves around the loop, it will be delayed by (D1 + D2) and its gain will be reduced by a factor of "g". Importantly, from "g" we can deduce how often the signal needs to travel around the loop until the gain has been reduced by 60 dB. Depending on the decay time set by the user, we then know what the sum of the delay times (D1 + D2) has to be on order exhibit the desired delay time. Thus, for any D1, we can set D2 accordingly. This means that the "decay" parameter is not tied to any delay times, but to the literal time value that is inserted into said formula that is then used to determine D2.
    
    If it is a multichannel loop, then I found that it works to use the average delay over all channels.

    I believe that allpass filters can be ignored for the purpuse of determining reverb time. If anyone finds evidence to the contrary I would be curious to find out. Also, if there are lowpass filters in the loop, then these will futher reduce the RT60 time. I have not found an easy way to factor this into the formula, so I chose to ignore this for my own purposes. In the end, it's just a number.

    Having said all this, TailMeasure didn't play quite as big of a role in implementing the decay parameter as I had initially thought it would. It was however invaluable for validating the results.
 
 ## Licensing
 
 TailMeasure is licensed under the GNU General Public License (GPL).
 
 
 
 
 
