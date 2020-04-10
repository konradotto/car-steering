![pipeline](https://git.chalmers.se/courses/dit638/students/group_13/badges/master/pipeline.svg)![License: MIT(https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

# Group 13

## Our Project

This section will eventually be used to describe what our project delivers, what our goals and use-cases are and what visions we have for its future.

## Team Members

Our team consists of the following 4 members.  
Each bulletpoint also refers to 
the student's GitLab-Id which is set up to link their preferred mail-address
for getting in touch:
- Arvin Esfahani Zadeh Sakhi Langeroudi - [sakhi](mailto:gusesfar@student.gu.se)
- Armin Ghoroghi - [arming](mailto:arre2118@gmail.com)
- Jean Paul Massoud - [jean](mailto:gusjeanma@student.gu.se)
- Konrad Otto - [konrado](mailto:gusottko@student.gu.se)

## Build Instructions

This section provides detailed instructions to help users new to our project to get everything set up and running in no time.

### Dependencies

We are providing a Dockerfile that will resolve all dependencies for you. In order to use Docker to build and run our project, set it up according to the instructions for your operating system:

* [Docker](https://docs.docker.com/install/)
* [Docker Compose](https://docs.docker.com/compose/install/)

The following dependencies are a part of the source distribution:      
* CMake (3.10.X)
* Make (4.1.X)
* g++ (7.5.X)

### Step-by-Step

With the previously listed minimal dependencies, you should be able to run our project by executing these steps in your terminal:

* Create new folder using `mkdir <FolderName>` from the terminal
* In the terminal `cd` in that folder
* Go to our repository on GitLab and copy the [SSH-Key](git@git.chalmers.se:courses/dit638/students/group_13.git) to clone via ssh or the [webaddress](https://git.chalmers.se/courses/dit638/students/group_13/-/tree/master)
* In the terminal, write `git clone <SSH-Key>` to clone that repo in a clean folder.
* In the clone, there should be a docker image included to assist with the build named dockerfile.
* Use `docker build -t <name>:latest -f Dockerfile .` to run the build. 
    * (-t is the tag, latest means latest version, -f specifies file to run)
* When the build is completed, a container has been created. To execute it you run `docker run --rm <name>:latest <input>` where input is the input you pass to the executable.

## Our Workflow

This section describes the workflow we have agreed on for both *Adding New Features* and *Fixing Bugs* with GitLab.

### Adding New Features
We have decided to follow these steps when adding new features to our project:

1. Create a milestone representing the final feature to be implemented, and specify the time frame.
2. Add an issue describing the sub-feature that solves part of the feature and link it to the milestone.
3. Assign a member to the created issue.
4. Create a feature branch that links to the issue i.e:
 `feature/<issue-number>-<issueDescription>`.
5. Create a pull request to merge the feature branch with the `develop` branch only if the latest commit on the feature branch passes the pipeline tests.
6. Once the code is reviewed and proved stable on `develop`, create a pull request to merge develop with `master`.
7. Delete the feature branch and close the issue.

### Fixing Bugs

Our workflow for fixing bugs or removing unwanted behaviour from our project consists of the following steps:

1. Detect a unwanted behaviour. This will usually happen one of the following ways:
    - Author observes unintended behaviour
    - Code review detects bug
    - Automated tests reveal bug
    - Manual test reveals bug
2. Take further steps to better understand the bug and be able to describe it as detailed as possible. These steps include but are not limited to:
    - Using a debugger (step-by-step execution and other helpful functionalities)
    - Running further tests
3. Open an [issue on GitLab](https://git.chalmers.se/courses/dit638/students/group_13/issues/new?issue%5Bassignee_id%5D=&issue%5Bmilestone_id%5D=#) to report the bug:
    - Pick a title for the bug
    - Choose our [BUG-template](.gitlab/issue_templates/BUG.md)
    - Fill the different sections to describe *Expected Behaviour*, *Current Behaviour*, suggest a *Possible Solution*, list *Steps to Reproduce*, give *Context* and more
4. Add a card on [Trello](https://trello.com/b/Gtc7wuxz/team-sprintboard) to solve the bug according to our process
5. Assign the bug (either immediately or during the next sprint planning). Ideally the solution involves:
    - Authors of the relevant code
    - Person who issued the bug-report
6. The assignees find the root cause of the unwanted behaviour and resolve it


## Rules for Commit Messages

Our rules for commit messages are inspired through the articles ["How to Write a Git Commit Message"](https://chris.beams.io/posts/git-commit/) by Chris Beams and ["How to Write Good Commit Messages: A Practical Git Guide"](https://www.freecodecamp.org/news/writing-good-commit-messages-a-practical-guide/) by Bolaji Ayodeji.  
We have written them down in accordance with the rules for their subject lines (except the 50 character limit):

- Write short subject lines for all commit messages
- Begin subject lines with a capitalized verb
- Write subject lines in imperative mood (e.g. “Fix formatting of file x”)
- End subject line after max 50 characters
- End subject lines without punctuation
- Separate subject line and optional body with a blank line
- Use the body to explain *what* and *why*
- Do not assume the reviewer understands what the original problem was; make sure to add it in the body
- Do not think your code is self-explanatory
- Wrap body at 72 characters
- Capitalize all paragraphs
- Use keywords to close, fix or resolve issues at the bottom of the body (e.g.: “Fixes: #666)

## License

We decided on using the [**MIT License**](LICENSE) for open source projects.
