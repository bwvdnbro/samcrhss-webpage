---
layout: page
link: Software
image: /assets/images/registration.png
---

# Recommended software

While most of the lectures during the summer school will focus on 
algorithms and techniques, there will also be plenty of hands on 
sessions in the afternoons during which you will get the opportunity to 
play with code yourself.

Below is a list of software that will be required for some of the hands 
on exercises. To make sure the focus of the hands on sessions is on the 
exercises themselves and not on software issues, we strongly recommend 
that you install this software on your laptop before the hands on 
sessions.

## Operating systems

While most of the software will work on all major operating systems, we 
strongly recommend using a Unix-based operating system like Linux or Mac 
OSX. On Windows, it is possible to emulate a Unix system using 
[VirtualBox](https://www.wikihow.com/Install-Ubuntu-on-VirtualBox) or 
[Docker](https://tutorials.ubuntu.com/tutorial/tutorial-windows-ubuntu-hyperv-containers#0) 
(Windows 10 only).

## Compilers

Most of the exercises will use Python or code written in Fortran or 
C(++). To compile the latter, you will require a Fortran and a C(++) 
compiler. On Linux, we recommend installing the [GCC 
compiler](http://gcc.gnu.org/). On Mac OSX, compilers are available 
through [Xcode](https://developer.apple.com/xcode/).

## Python

Python is a scripting language that is very popular in astronomy. It 
consists of a main Python program that translates code input into 
machine code at runtime, and a large repository of additional libraries 
(*modules*) that provide additional features. The main Python program 
should be available by default on most Linux and Mac OSX systems. The 
additional modules need to be installed in a way that is consistent with 
the main Python installation to avoid version conflicts. There are two 
recommended ways to do this:

1. Use the Python installation that ships with your operating system and 
install additional modules using the Python package installer, `pip` 
([detailed 
reference](https://packaging.python.org/tutorials/installing-packages/)) 
2. Install a Python distribution like 
[Anaconda](https://www.anaconda.com/download/) or [Enthough 
Canopy](https://www.enthought.com/products/canopy). These distributions 
provide an alternative main Python program and contain their own package 
manager that can be used to install additional modules.

Independent of how you choose to install Python, we recommend installing 
Python 3 (Python 2 will be discontinued 
[soon](https://pythonclock.org/)) with at least the following additional 
modules:

* `numpy`: support for multidimensional arrays, basic numerical functions...
* `scipy`: advanced mathematical functions, statistics, numerical integration,
optimisation...
* `matplotlib`: data visualisation
* `jupyterlab`: support for interactive Python notebooks

## Visualisation software

To visualise large 3D data cubes, we recommend installing 
[VisIt](https://wci.llnl.gov/simulation/computer-codes/visit/).

## Astronomical software

Large astronomical instruments like ALMA require dedicated software to 
handle the complex post-processing of their data products. For one of 
the hands on exercises, you will need to use one of these software 
applications, [CASA](https://casa.nrao.edu/), to create synthetic ALMA 
observations. Note that CASA officially does not support any Linux 
distributions other than Red Hat 6 and 7. Despite this, it is still 
possible to run it on most other Linux distributions; simply download 
the latest release tar-ball for Red Hat 6 or 7 and untar it in a 
location of choice (see [these 
instructions](https://casa.nrao.edu/casadocs/casa-5.6.0/introduction/obtaining-and-installing)).
