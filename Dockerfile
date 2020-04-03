# Copyright (C) 2020  Christian Berger
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Which Docker image shall be used on the GitLab runner?
image: docker:19.03.3

# Details about how to connect to the Docker service to run this build.
variables:
  DOCKER_HOST: tcp://172.17.0.1:2375
  DOCKER_TLS_CERTDIR: ""

services:
  - name: docker:19.03.3-dind

stages:
  - build
  - deploy

# Display information before we start the build.
before_script:
  - docker info
  - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY

# This section describes what shall be done to build and test the project.
build-and-test:
  tags:
    - docker-build
  stage: build
  only: ['branches']
  script:
    - docker build -f Dockerfile .


# This section describes what shall be done to deploy artefacts from the project.
release:
  tags:
    - docker-build
  stage: deploy
  when: on_success
  only:
    refs:
      - tags
    variables:
      - $CI_COMMIT_TAG =~ /^\Av(\d)\.(\d)\.(\d)\z/
  variables:
    IMAGE: $CI_REGISTRY_IMAGE/$CI_COMMIT_REF_SLUG:$CI_COMMIT_SHA
  script:
    - docker build -t $IMAGE -f Dockerfile .
    - docker push $IMAGE
    
