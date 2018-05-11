# jz18sub
Json/Zmq 2018 Submission/Details

### Notes ###
- Primary Linux Development, but cross platform libraries and tools.

- Automation Director - master control program. database agnostic?  use in  memory tables or sqlite.
- Resource App - used to proxy something - rti/hsm/filetran.  comm with deal/router
- Processing App - pull from resource apps with pub/sub list
- How we use and convert structures/values with json.


About Json/Zmq and application network for financial transactions.

Why ZMQ?  
- Advantages

Why json? 
- Zmq is all CHAR
- Database is all CHAR



Code Requirements
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


So you start a new position and the first question is along the lines of 'we are rebuilding our systems, ZMQ, RabbitMQ, or Protobuffers which do you like more?' ummmmm let me look?

We are going to walk through using ZMQ and JSON to build a messaging platform which we use to help us process credit card transactions.
Our structure is made up of a primary daemon as a MCP.  
Then we have individual resources or processing apps that when started register with the MCP.  
We will go over how and why we found using JSON was the easiest and most effective, even with converting everything to strings.


- Background: Why did you do this?
  - What is the problem you are addressing? How are things done today?
  Speed and performance.  Using single threaded window applications running multiple instances.  Moderning sections of older software, a rewrite as a refactor.
  - What is the difficulty and why?  
  Inefficent use of rsources for modern operating systems.  a lot of database dependency.  old systems to integrate with.
  - Why has this problem not been solved before?
    Similar problems have been solved like this before, but not in-house for us.  There are several libraries we were able to use to help us reach a desired impact in less time.  Thats what we are here to discuss.
  - What did you expect or what is the hypothesis?
  When upgrading software from the level we were, performance increases were a given.  But that was a bias we owuld find would catch us. 
- Approach: What did you do?
  -What kind of data did you collect and how did you collect it?
  Transaction logs of existing running applications.
  - How did you assess the data?
  Timing primarily as its a measure as a transaction processor.
  - Who were the subjects? How many?
  - Is this a case study? An experience report? A pilot? Did you set up experiments?
- Findings: What did you learn or discover?
  - Did a new technique or approach work or not?
  We found implmenting zmq library along with json gave us a fast flexible and easy way to move data between the different applications on the network.
  - What new information or data is provided?
  We give an example of implmenting a robust framework in a financial network.
  - What was confirmed or refuted?
  ZMQ and json was able to help us speed up our networks by reducing data (vs xml) and was easy to implement in modern c++
  - Were there surprises?
  Not everything we wrote was a speed increase.  Threads have to be managed.  Timing is always an issue.
- Conclusion: What does it mean?
  - What can we now do differently or that we couldn’t do before?
  - Who will be affected by these results? Does this apply to team members? Team leads? Coaches? Other stakeholders?
  - Will something be cheaper, faster, or better?
