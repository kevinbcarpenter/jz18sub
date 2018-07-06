# jz18sub
## Json/Zmq 2018 Submission/Details ##

We are going to walk through using ZMQ and JSON to build a messaging platform which we use to help us process credit card transactions.
Our structure is made up of a primary daemon as a MCP.  
Then we have individual resources or processing apps that when started register with the MCP.  
We will go over how and why we found using JSON was the easiest and most effective, even with converting everything to strings.


### Notes ###
- Primary Linux Development, but cross platform libraries and tools.

- Master Control Program - database agnostic?  use in  memory tables or sqlite.
- Resource App - used to proxy something - rti/hsm/filetran.  comm with deal/router
- Processing App - pull from resource apps with pub/sub list
- How we use and convert structures/values with json.

About Json/Zmq and application network for financial transactions.

#### Why ZMQ?  ####
- Advantages

#### Why json? ####
- Zmq is all CHAR
- Database is all CHAR

#### Code Requirements ####
- Library Base
  - App class
  - ZmqHandler
  - ZBSocket
  - Conversion Utilities (Safe String)
- Automation 
  - SQLite DB
- Resource App
  - HSMProxy/RTI
  - File Read/Write
- Compute/Processing App
  - GPGPU
  - In/Out


##### Other Projects Used ######
Here are a couple other projects I reference for ease.

- [ZMQ PP - C++ Interace for libzmq](https://github.com/zeromq/zmqpp)
- [JSON for Modern C++](https://github.com/nlohmann/json)
- [SpdLog - Fast header only logger](https://github.com/gabime/spdlog)


### Outline ###

Quickly: Transactions  
- What is it? Types (card/emv/web).  
- What does this have to do with when I swipe my card at the grocery store? 
- Example of credit card data transactions routing through the network. 
- Quickly demonstrate settlement pieces we re-created in our example code. 
- PCI surrounds everything we do. 

Background: Why did you do this?
- We are addressing speed, performance, and maintainability.   Modernizing sections of older software, we have a rewrite done as a refactor thanks to our applications being very modular.  Single threaded Windows applications running multiple instances is inefficient.   
- The difficulty in the current system is inefficient use of resources for modern operating systems. We have a lot of database dependency and reliance on stored procedures. Further Visa/MC systems have some legacy (MML/EBCDIC) systems to integrate with. 
  - Also... Visa/MC specs are massive.  Just EMV has its own books and site emvco.com and it's primarily just chip! 
- Looking at similar problems solved previous we looked for the shortest path to solutions. There are several libraries we were able to use to help us reach a desired impact in less time which we will show. 
- When upgrading software from the level we were, performance increases were a given.  However, that was a bias we would find would catch us if we were not careful.  Every line of legacy code added as a bug fix, makes it harder to translate in new code. 

Approach: What did you do? 
- Current systems are modular, we started with a resource management application for Hardware Security Module (HSM).  Followed by processing applications (Settlement) and finally application/system management. 
- We use timing as the primary performance measure as a transaction processor. 
- Using lots of logging we looked at existing timing and code and designed new fixes, incremental in some cases. 
  - HSM Manager was singled threaded – now its queued and multithreaded. 
- Each app was built in the support of the next. 
  - Cryptographic hardware is required to manage card data. PCI. 
  - Settlement app updated next to use new HSM Manager. 
  - Control app created to managed resources and processing applications. 

Findings: What did you learn or discover? 
- We found implementing ZMQ library along with JSON gave us a fast flexible and easy way to move data between the different applications on the network. 
- We give an example of implementing a robust framework in a financial network. 
- Due to OS limitations in production we were originally bound by C++98.  We moved to C++11 but then found we had need of C++14 features and once again had to move.  This entailed managing compiler and library differences in older production OS. 
- ZMQ and JSON was able to help us speed up our networks by reducing data (vs xml) and was easy to implement in modern C++. 
- Not everything we wrote was a speed increase. Threads have to be managed. Timing is always an issue. ZMQ will wait for a connection to come up and continue, can make testing interesting when something comes up with 20 requests waiting and floods the new app. 

Conclusion: What does it mean? 
- In our previous configuration we had a A/B failover. Now we can have multiple redundancies across the entire platform. 
- Clients will be able to receive more responses faster. Our parent company is able to onboard more clients. 
- Will something be cheaper, faster, or better? YES! 


#### Feedback
----------------------- REVIEW ---------------------
The abstract does not do a good job on selling this talk, which might be fairly interesting to the audience.

The github does not contain code, no idea which code will be shown in the presentation. The interfaces of ZMQ are C, but it seems a C++ wrapper was used.

I'm a bit afraid that this talk will only present glue code and few simple use cases of the named libraries in the github repository.

So while I like the Idea of the talk, the abstract does a poor job in clearly representing it.


----------------------- REVIEW ---------------------
The subject is interesting and the talk seems like one that many people will be at least technically interested in - although some may have principled objections.

I do wonder in how far you can talk about this without breaking an NDA. You're referring to MC and Visa details, as well as explaining things from your job site. This makes the talk for me a very risky one to accept...


----------------------- REVIEW ---------------------
This talk is about performance, security, scalability, redundancy, and refactoring of a legacy code base in the finance world. Awesome. I want to learn about what the speaker did.

However, looking at the abstract, IMHO there is just no way to cover all of that in just one hour in reasonable quality. Three hours would be the minimum.

If the talk gets accepted, I seriously recommend multiple test runs to prune the talk and focus on just a subset of the outlined topics.


----------------------- REVIEW ---------------------
Some aspects here are quite fascinating to me. IMHO there is nearly always no justification to re-write a working/production code (non-trivial size) from C++X to C++X++. So it would be very interesting to see at what point the technical debt overwhelmed the costs of refactoring/re-write (and the additional costs of testing and the additional risks of some bugs slipping through).

From the abstract, it seems like the audience needs quite some more context before getting to the core topic.

I can see the work done is not simple - but I don't see how this would be interesting as a 30 or 60 min talk at cppcon.


----------------------- REVIEW ---------------------
From what I gather from the outline, this seems to be more of a lessons-learned talk, but I'm not sure how relevant the talk is to C++ overall rather than a message model selection problem. I also feel like this talk is slightly too tailored to a single domain; what can the average C++ developer get from attending this talk?


----------------------- REVIEW ---------------------
It is not clear from the title and the abstract how this talk would directly benefit C++ developers. It talked about how JSON is a better choice over XML, and looks like a lot of talking about the background and why. If the focus could be shifted onto more on how C++ was used and the move to C++ 11 and C++ 14, I could see this being an interesting case study.


----------------------- REVIEW ---------------------
Recent business requirements have tended to increase the desire for scalable RPC mechanisms. This talk directly addresses those needs by way of implementing the solution in C++. The crux of the talk however appears to be "what we learned while creating/revisiting a scalable RPC solution with respect to serialization" (i.e. XML vs JSON) and there is also some alluding to simplification of the RPC API.

The overall proposal seems to center not on C++ as offering anything new other than the medium in which the solution was created. Discussion of benefiting from C++14 could be a compelling aspect of this talk, but it appears the talk is really about solving/developing a scalable RPC solution rather than anything specific to C++.


----------------------- REVIEW ---------------------
I'm not sure if this talk will atract a lot of attendees. There is a lot of financial world's specifics. I know the abbrevations just because I was shortly working next to a team who needed to do a system which was PCI specific. There is not much C++ specifics.





#### Resources ####
[HSM - Hardware Security Module](https://safenet.gemalto.com/data-encryption/hardware-security-modules-hsms)
Hardware for managing cryptographic keys in a secure environment.

[HSM - Wikipedia](https://en.wikipedia.org/wiki/Hardware_security_module)
More details on HSM's.

[EMVCO](http://www.emvco.com)
Europay/Visa/Mastercard Chip specification.

