<img src="logo/bustub-whiteborder.svg" alt="BusTub Logo" height="200">

-----------------

[![Build
Status](https://github.com/cmu-db/bustub/actions/workflows/cmake.yml/badge.svg)](https://github.com/cmu-db/bustub/actions/workflows/cmake.yml)

BusTub is a relational database management system built at [Carnegie Mellon
University](https://db.cs.cmu.edu) for the [Introduction to Database
Systems](https://15445.courses.cs.cmu.edu) (15-445/645) course. This system was developed
for educational purposes and should not be used in production environments.

BusTub supports basic SQL and comes with an interactive shell. You can get it running
after finishing all the course projects.

<img src="logo/sql.png" alt="BusTub SQL" width="400">

**WARNING: IF YOU ARE A STUDENT IN THE CLASS, DO NOT DIRECTLY FORK THIS REPO. DO NOT PUSH
PROJECT SOLUTIONS PUBLICLY. THIS IS AN ACADEMIC INTEGRITY VIOLATION AND CAN LEAD TO
GETTING YOUR DEGREE REVOKED, EVEN AFTER YOU GRADUATE.**

We make the autograder for each assignment available to non-CMU students on Gradescope
after their due date for CMU students. In exchange for making this available to the
public, we ask that you do not make your project implementations public on Github or other
source code repositories. Please read the course FAQ on how to use the autograder on
Gradescope. Run `python3 gradescope_sign.py` to sign an agreement before submitting to the
autograder.

**WARNING: IF YOU ARE A STUDENT OUTSIDE CMU, DO NOT MAKE YOUR SOLUTION PUBLICLY AVAILABLE,
AND DO SUBMIT YOUR OWN WORK. OTHERWISE, YOU WILL BE BANNED FROM USING THE AUTOGRADER.**
Thank you for creating a fair learning environment.

## Cloning this Repository

The following instructions are adapted from the Github documentation on [duplicating a
repository](https://docs.github.com/en/github/creating-cloning-and-archiving-repositories/creating-a-repository-on-github/duplicating-a-repository).
The procedure below walks you through creating a private BusTub repository that you can
use for development.

1. Go [here](https://github.com/new) to create a new repository under your account. Pick a
   name (e.g. `bustub-private`) and select **Private** for the repository visibility
   level.
2. On your development machine, create a bare clone of the public BusTub repository:
   ```console
   $ git clone --bare https://github.com/cmu-db/bustub.git bustub-public
   ```
3. Next, [mirror](https://git-scm.com/docs/git-push#Documentation/git-push.txt---mirror)
   the public BusTub repository to your own private BusTub repository. Suppose your GitHub
   name is `student` and your repository name is `bustub-private`. The procedure for
   mirroring the repository is then:
   ```console
   $ cd bustub-public
   
   # If you pull / push over HTTPS
   $ git push https://github.com/student/bustub-private.git master

   # If you pull / push over SSH
   $ git push git@github.com:student/bustub-private.git master
   ```
   This copies everything in the public BusTub repository to your own private repository.
   You can now delete your local clone of the public repository:
   ```console
   $ cd ..
   $ rm -rf bustub-public
   ```
4. Clone your private repository to your development machine:
   ```console
   # If you pull / push over HTTPS
   $ git clone https://github.com/student/bustub-private.git

   # If you pull / push over SSH
   $ git clone git@github.com:student/bustub-private.git
   ```
5. Add the public BusTub repository as a second remote. This allows you to retrieve
   changes from the CMU-DB repository and merge them with your solution throughout the
   semester:
   ```console
   $ git remote add public https://github.com/cmu-db/bustub.git
   ```
   You can verify that the remote was added with the following command:
   ```console
   $ git remote -v
   origin	https://github.com/student/bustub-private.git (fetch)
   origin	https://github.com/student/bustub-private.git (push)
   public	https://github.com/cmu-db/bustub.git (fetch)
   public	https://github.com/cmu-db/bustub.git (push)
   ```
6. You can now pull in changes from the public BusTub repository as needed with:
   ```console
   $ git pull public master
   ```
7. **Disable GitHub Actions** from the project settings of your private repository,
   otherwise you may run out of GitHub Actions quota.
   ```
   Settings > Actions > General > Actions permissions > Disable actions.
   ```

We suggest working on your projects in separate branches. If you do not understand how Git
branches work, [learn
how](https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging). If you
fail to do this, you might lose all your work at some point in the semester, and nobody
will be able to help you.

## Build

We recommend developing BusTub on Ubuntu 22.04, or macOS (M1/M2/Intel). We do not support
any other environments (i.e., do not open issues or come to office hours to debug them).
We do not support WSL. The grading environment runs Ubuntu 22.04.

### Linux (Recommended) / macOS (Development Only)

To ensure that you have the proper packages on your machine, run the following script to
automatically install them:

```console
# Linux
$ sudo build_support/packages.sh
# macOS
$ build_support/packages.sh
```

Then run the following commands to build the system:

```console
$ mkdir build
$ cd build
$ cmake ..
$ make
```

If you want to compile the system in debug mode, pass in the following flag to cmake:
Debug mode:

```console
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
$ make -j`nproc`
```
This enables [AddressSanitizer](https://github.com/google/sanitizers) by default.

If you want to use other sanitizers,

```console
$ cmake -DCMAKE_BUILD_TYPE=Debug -DBUSTUB_SANITIZER=thread ..
$ make -j`nproc`
```

There are some differences between macOS and Linux (i.e., mutex behavior) that might cause
test cases to produce different results in different platforms. We recommend students to
use a Linux VM for running test cases and reproducing errors whenever possible.


# Special notes
clang-tidy does not work on local MacOS. The only working solution is to manually run
clang-tidy on each file. For example:
```
clang-tidy -p=build src/buffer/lru_k_replacer.cpp --header-filter='' -fix
```