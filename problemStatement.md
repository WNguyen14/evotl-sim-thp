eVTOL Simulation Problem

### Objective

Show us your awesome skills! We want to see your design, coding, documentation, testing, and communication abilities first hand!

### Details

This work must be coded in **C++,** and submitted through GitHub, BitBucket, or Gitlab. Upload the documented code to your personal account and send us a link. We may look at the git history to see how you use version control.

At Joby we love good OOP design. Please exercise the basic tenets of OOP, using relevant principles where appropriate. We’re looking for robust, testable, modular, and well-documented code.

Unit Testing is a big part of our development process. Please include just a few examples of unit tests or how your code is testable. Tests don't necessarily need to run in any test framework, but please be ready to talk about what and how you tested your code.  
<br/>To dos and notes are OK. If you want to incorporate an idea that makes sense in a real project, but would take too much time to implement, just leave a note. For example:

// Todo: Refactor database to use hashmap rather than raw array for

// &lt;xyz&gt; reasons..

### Problem Statement

There are five companies developing eVTOL aircraft. The vehicle produced by each manufacturer has different characteristics. Six distinct properties are laid out in the below table:

| Company Name | Alpha Company | Bravo Company | Charlie Company | Delta Company | Echo Company |
| --- | --- | --- | --- | --- | --- |
| Cruise Speed (mph) | 120 | 100 | 160 | 90  | 30  |
| Battery Capacity (kWh) | 320 | 100 | 220 | 120 | 150 |
| Time to Charge (hours) | 0.6 | 0.2 | 0.8 | 0.62 | 0.3 |
| Energy use at Cruise (kWh/mile) | 1.6 | 1.5 | 2.2 | 0.8 | 5.8 |
| Passenger Count | 4   | 5   | 3   | 2   | 2   |
| Probability of fault per hour | 0.25 | 0.10 | .05 | .22 | .61 |

You will simulate using these vehicle for 3 hours. Of course your simulation should take much less time than that. 20 total vehicles should be deployed, and a random number of each type of vehicle should be used (with the total between all five types being 20).

There are only three chargers available for all 20 vehicles! A single charger can only be used by one vehicle at a time. Assume the chargers can charge the batteries in the _Time to Charge_ time listed for each vehicle.

Keep track of the following statistics _per vehicle type_:  
\- average flight time per flight  
\- average distance traveled per flight  
\- average time charging per charge session  
\- total number of faults  
\- total number of passenger miles.  
For example, if there are 2 vehicles carrying 4 passengers on a vehicle that cruises for 1 hour at 100 mph, total number of passenger miles is 2 \* 4 \* 1 \* 100 = 800.

Assume that:  
\- Each vehicle starts the simulation with a fully-charged battery  
\- Each vehicle instantaneously reaches Cruise Speed

\- Each vehicle is airborne for the full use of the battery, and is immediately in line for the charger after running out of battery power.

Please include the statistics recorded during at least one run of the simulation in your submission.

Please do not hesitate to reach out to ask any questions about the problem! However, if you find yourself making an assumption that we haven’t explained explicitly, you may simply document your assumption. It’s interesting to us to hear what assumptions you made and why.