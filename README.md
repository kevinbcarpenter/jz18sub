# jz18sub
## Json/Zmq 2018 Submission/Details ##

We are going to talk about how using ZMQ and JSON to build a messaging platform we improved the speed of processing credit card transactions.
Our demo apps will be a primary daemon we will call MCP which connects various individual resources or processing apps that when started register with the MCP.
We will go over how and why we found using JSON was the easiest and most effective, even with converting everything to strings.
Why C++ 14 made parts easier then previous versions.


### Notes ###
- Primary Linux Development, but cross platform libraries and tools.
- Master Control Program - a scaled down version of our internal automation commander.  Handles all the registration, heartbeat and statementament.
- workHourse - is the base of the application platform.
- How we use and convert structures/values with json.

#### Why ZMQ?  ####
- Advantages
- CZMQ
- ZMQCPP

#### Why json? ####
- Zmq is all CHAR
- Database (Oracle DataBuffers uses all CHAR).

##### Other Projects Used ######
Here are a couple other projects I reference for ease.
- [JSON for Modern C++](https://github.com/nlohmann/json)
- [ZMQ PP - C++ Interace for libzmq](https://github.com/zeromq/zmqpp)
- [Args](https://github.com/Taywee/args)

### Outline ###
Quickly: Transactions  
- What is it? Types (card/emv/web).  
- Example of credit card data transactions routing through the network. 
- PCI surrounds everything we do. 

#### Background: Why did you do this?
- We are addressing speed, performance, and maintainability.   Modernizing sections of older software, we have a rewrite done as a refactor thanks to our applications being very modular.  Single threaded Windows applications running multiple instances is inefficient.   
- The difficulty in the current system is inefficient use of resources for modern operating systems. We have a lot of database dependency and reliance on stored procedures. Further Visa/MC systems have some legacy (MML/EBCDIC) systems to integrate with. 
- Looking at similar problems solved previous we looked for the shortest path to solutions. There are several libraries we were able to use to help us reach a desired impact in less time which we will show. 
- When upgrading software from the level we were, performance increases were a given.  However, that was a bias we would find would catch us if we were not careful.  Every line of legacy code added as a bug fix, makes it harder to translate in new code. 

#### Approach: What did you do? 
- Current systems are modular, we started with a resource management application for Hardware Security Module (HSM).  Followed by processing applications (Settlement) and finally application/system management. 
- We use timing as the primary performance measure as a transaction processor. 
- Using lots of logging we looked at existing timing and code and designed new fixes, incremental in some cases. 
  - HSM Manager was singled threaded – now its queued and multithreaded. 
- Each app was built in the support of the next. 
  - Cryptographic hardware is required to manage card data. PCI. 
  - Settlement app updated next to use new HSM Manager. 
  - Control app created to managed resources and processing applications. 

#### Findings: What did you learn or discover? 
- We found implementing ZMQ library along with JSON gave us a fast flexible and easy way to move data between the different applications on the network. 
- We give an example of implementing a robust framework in a financial network. 
- Due to OS limitations in production we were originally bound by C++98.  We moved to C++11 but then found we had need of C++14 features and once again had to move.  This entailed managing compiler and library differences in older production OS. 
- ZMQ and JSON was able to help us speed up our networks by reducing data (vs xml) and was easy to implement in modern C++. 
- Not everything we wrote was a speed increase. Threads have to be managed. Timing is always an issue. ZMQ will wait for a connection to come up and continue, can make testing interesting when something comes up with 20 requests waiting and floods the new app. 

#### Conclusion: What does it mean? 
- In our previous configuration we had a A/B failover. Now we can have multiple redundancies across the entire platform. 
- Clients will be able to receive more responses faster. Our parent company is able to onboard more clients. 
- Will something be cheaper, faster, or better? YES! 


#### Feedback
----------------------- REVIEW ---------------------
- I seriously recommend multiple test runs to prune the talk and focus on just a subset of the outlined topics.
- I don't see how this would be interesting as a 30 or 60 min talk at cppcon.
- more of a lessons-learned talk, but I'm not sure how relevant the talk is to C++ overall; what can the average C++ developer get from attending this talk?
- If the focus could be shifted onto more on how C++ was used and the move to C++ 11 and C++ 14
- Discussion of benefiting from C++14 could be a compelling aspect of this talk
- There is not much C++ specifics.



#### Resources ####
[HSM - Hardware Security Module](https://safenet.gemalto.com/data-encryption/hardware-security-modules-hsms)
Hardware for managing cryptographic keys in a secure environment.

[HSM - Wikipedia](https://en.wikipedia.org/wiki/Hardware_security_module)
More details on HSM's.

[EMVCO](http://www.emvco.com)
Europay/Visa/Mastercard Chip specification.

