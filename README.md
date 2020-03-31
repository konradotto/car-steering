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

### Bugfixing


## Rules for Commit Message


