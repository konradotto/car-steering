# Group 13

## Our Project


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

### Dependencies
* Cmake (3.10.X)
* Make (4.1.X)
* G++ (7.5.X)
* Docker (19.03.X)
* Docker-compose (1.25.X)
* Linux OS

### Step-by-Step
* Create new folder using `mkdir <FolderName>` from the terminal
* In the terminal `cd` in that folder
* Go to gitlab and copy the SSH-Key
* In the terminal, write `git clone <SSH-Key>` to clone that repo in a clean folder.
* In the clone, there should be a docker image included to assist with the build named dockerfile.
* Use ` docker build -t <YourCID>:latest -f Dockerfile .` to run the build. 
    * (-t is the tag, latest means latest version, -f specifies file to run)
* When build is complete a container has been made and to execute it you run `docker run --rm <YourCID>:latest <input>`

## Our Workflow

### Adding New Features
1. Create a milestone representing the final feature to be implemented, and specify the time frame.
2. Add an issue describing the sub-feature that solves part of the feature and link it to the milestone.
3. Assign a member to the created issue.
4. Create a feature branch that links to the issue i.e:
 ` feature / <issueDescription> - <issue-number> `.
5. Create a pull request to merge the feature branch with the `develop` branch only if the latest commit on the feature branch passes the pipeline tests.
6. Once the code is reviewed and proved stable on `develop`, create a pull request to merge develop with `master`.
7. Delete the feature branch and close the issue.

### Bugfixing

Our workflow for bugfixing or the removal of unwanted behaviour from our project contains the following steps:

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

Our rules for commit messages are inspired through the article on ["How to Write a Git Commit Message"](https://chris.beams.io/posts/git-commit/) by Chris Beams.  
We have written them down in accordance with the rules for their subject lines:

- Write short subject lines for all commit messages
- Begin subject lines with a capitalized verb
- Write subject lines in imperative mood (e.g. “Fix formatting of file x”)
- End subject lines without punctuation
- Separate subject line and optional body with a blank line
- End subject line after max 50 characters
- Wrap body at 72 characters
- Use keywords to close, fix or resolve issues from the body (e.g.: “Fixes #666)

## License

We decided on using the [**MIT License**](LICENSE) for open source projects.