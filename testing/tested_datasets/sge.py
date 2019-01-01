#!/usr/bin/python

#
# sge.py
#
# Creative Commons Attribution License
# http://creativecommons.org/licenses/by/2.5/
#
# Trevor Strohman
#    First release: December 2005
#    This version:  11 January 2006
#
# Bug finders: Fernando Diaz
#

"""Submits jobs to Grid Engine.
Handles job dependencies, output redirection, and job script creation.
"""


import os
import os.path
import time

class Job:
  def __init__( self, name, command, queue=None ):
    self.name = name
    self.queue = queue
    self.command = command
    self.script = command
    self.dependencies = []
    self.submitted = 0

  def addDependency( self, job ):
    self.dependencies.append( job )

  def wait( self ):
    finished = 0
    interval = 5
    while not finished:
      time.sleep(interval)
      interval = min( 2 * interval, 60 )
      finished = os.system( "qstat -j %s > /dev/null" % (self.name) )

class JobGroup:
  def __init__( self, name, command, queue=None, arguments={} ):
    self.name = name
    self.queue = queue
    self.command = command
    self.dependencies = []
    self.submitted = 0
    self.arguments = arguments
    self.generateScript()

  def generateScript( self ):
    self.script = ""
    # total number of jobs in this group
    total = 1

    # for now, SGE_TASK_ID becomes TASK_ID, but we base it at zero
    self.script += """let "TASK_ID=$SGE_TASK_ID - 1"\n"""

    # build the array definitions
    for key in self.arguments.keys():
      values = self.arguments[key]
      line = ("%s_ARRAY=( " % (key))
      for value in values:
        line += "\'"
        line += value
        line += "\' "
      line += " )\n"
      self.script += line
      total *= len(values)
    self.script += "\n"

    # now, build the decoding logic in the script
    for key in self.arguments.keys():
      count = len(self.arguments[key])
      self.script += """let "%s_INDEX=$TASK_ID %% %d"\n""" % ( key, count )
      self.script += """%s=${%s_ARRAY[$%s_INDEX]}\n""" % ( key, key, key )
      self.script += """let "TASK_ID=$TASK_ID / %d"\n""" % ( count )

    # now, run the job
    self.script += "\n"
    self.script += self.command
    self.script += "\n"

    # set the number of tasks in this group
    self.tasks = total

  def addDependency( self, job ):
    self.dependencies.append( job )

  def wait( self ):
    finished = 0
    interval = 5
    while not finished:
      time.sleep(interval)
      interval = min( 2 * interval, 60 )
      finished = os.system( "qstat -j %s > /dev/null" % (self.name) )

def build_directories( directory ):
  subdirectories = [ "output", "stderr", "stdout", "jobs" ];
  directories = [ os.path.join( directory, subdir ) for subdir in subdirectories ]
  needed = filter( lambda x: not os.path.exists( x ), directories )
  map( os.mkdir, needed )

def build_job_scripts( directory, jobs ):
  for job in jobs:
    scriptPath = os.path.join( directory, "jobs", job.name )
    scriptFile = file( scriptPath, "w" )
    scriptFile.write( "#!/bin/sh\n" )
    scriptFile.write( "#$ -S /bin/bash\n" )
    scriptFile.write( "ulimit -t 86400\n")
    scriptFile.write( job.script + "\n" )
    scriptFile.close()
    os.chmod( scriptPath, 0755 )
    job.scriptPath = scriptPath

def extract_submittable_jobs( waiting ):
  """Return all jobs that aren't yet submitted, but have no dependencies that have
  not already been submitted."""
  submittable = []

  for job in waiting:
    unsatisfied = sum([(subjob.submitted==0) for subjob in job.dependencies])
    if unsatisfied == 0:
      submittable.append( job )

  return submittable

def submit_safe_jobs( directory, jobs ):
  """Submits a list of jobs to Grid Engine, using the directory structure provided to store
  output from stderr and stdout."""
  for job in jobs:
    job.out = os.path.join( directory, "stdout" )
    job.err = os.path.join( directory, "stderr" )

    args = " -N %s " % (job.name)
    args += " -o %s -e %s " % (job.out, job.err)
    args += " -cwd "
    #args += " -o . -e . "

    if job.queue != None:
      args += "-q %s " % job.queue

    if isinstance( job, JobGroup ):
      args += "-t 1:%d " % ( job.tasks )

    if len(job.dependencies) > 0:
      args += "-hold_jid "
      for dep in job.dependencies:
        args += dep.name + ","
      args = args[:-1]

    qsubcmd = ("qsub %s %s" % (args, job.scriptPath)) 
    print qsubcmd
    os.system( qsubcmd )
    job.submitted = 1
    
def run_safe_jobs( directory, jobs ):
  """In the event that Grid Engine is not installed, this program will
  run jobs serially on the local host."""
  for job in jobs:
    job.out = os.path.join( directory, "stdout" )
    job.err = os.path.join( directory, "stderr" )

    commands = []
    if isinstance( job, JobGroup ):
      for task in range(1,job.tasks+1):
        command = "export SGE_TASK_ID=%d; %s" % (task, job.scriptPath)
        commands.append(command)
    else:
      commands.append(job.scriptPath)

    count = 0
    for command in commands:
      print "# %s" % (command)
      command += " 2>%s/%s.%d >%s/%s.%d" % (job.err, job.name, count, job.out, job.name, count)
      os.system(command)
      count += 1
    job.submitted = 1
    
def check_for_qsub():
  for directory in os.environ['PATH'].split(':'):
    if os.path.isfile(os.path.join(directory, "qsub")):
      return True
  return False

def submit_jobs( directory, jobs, use_grid_engine=True ):
  waiting = list(jobs)

  while len(waiting) > 0:
    # extract submittable jobs
    submittable = extract_submittable_jobs( waiting )

    # run those jobs
    if use_grid_engine:
      submit_safe_jobs( directory, submittable )
    else:
      run_safe_jobs( directory, submittable )

    # remove those from the waiting list
    map( waiting.remove, submittable )


def build_submission( directory, jobs ):
  # check to see if qsub exists
  use_grid_engine = check_for_qsub()
  
  # build all necessary directories
  build_directories( directory )

  # build job scripts
  build_job_scripts( directory, jobs )

  # submit the jobs
  submit_jobs( directory, jobs, use_grid_engine )

