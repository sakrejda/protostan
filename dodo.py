from doit import create_after

def download_file(instructions, path='.'):
  import requests, os, pathlib
  pathlib.Path(path).mkdir(parents=True, exist_ok=True) 

  full_url = instructions['full_url']
  file_path = os.path.join(path, instructions['file_name'])

  r = requests.get(full_url, stream=True)
  with open(file_path, 'wb') as fd:
    for chunk in r.iter_content(chunk_size=128):
      fd.write(chunk)

def git_clone(instructions, path='.'):
  import subprocess, pathlib, os
  target = os.path.join(path, instructions['target_directory'])
  if not pathlib.Path(target).exists():
    subprocess.call(['git clone {} {} --recursive'.format(instructions['full_url'],
      target)], shell=True)

def extract_file(instructions, path='.'):
  import tarfile, zipfile, pathlib, os, shutil
  pathlib.Path(path).mkdir(parents=True, exist_ok=True) 

  compressed_file = os.path.join(path, instructions['file_name'])
  extracted_name = os.path.join(path, instructions['extracted_name'])
  target_directory = os.path.join(path, instructions['target_directory'])

  if compressed_file.endswith('tar.gz'):
    with tarfile.open(compressed_file) as tf:
      tf.extractall(path=path)
  elif compressed_file.endswith('.zip'): 
    with zipfile.ZipFile(compressed_file, mode='r') as zf:
      zf.extractall(path=path)
  else:
    raise ValueError("Path did not contain a known extension.")
  if pathlib.Path(target_directory).exists():
    shutil.rmtree(target_directory)
  shutil.move(extracted_name, target_directory)
  
def get_instructions():
  import yaml
  with open("download-instructions.yaml", 'r') as stream:
    instructions = yaml.load(stream)
  return instructions

def get_schema(directory):
  import os
  from fnmatch import fnmatch
  schema_list = []
  for f in os.listdir(directory):
    if fnmatch(f, '*.capnp'):
      schema_list.append(f)
  return schema_list

def task_construct_download_instructions():
  """Build download instruction files"""
  import yaml
  import io

  instructions = get_instructions()
  instruction_targets = [s + ".yaml" for s in instructions.keys()]

  def python_write_instructions(instructions, targets):
    for target in targets:
      key = target.split('.')[0]
      with io.open(target, 'w', encoding='utf8') as o:
        yaml.dump(instructions[key], o, default_flow_style=False, allow_unicode=True)

  return {
    'actions' : [(python_write_instructions, [], {'instructions' : instructions})],
    'file_dep' : ['download-instructions.yaml'],
    'targets' : instruction_targets
  }

def task_download_tooling():
  """Download and extract external projects."""
  import requests, yaml, os

  instructions = get_instructions()
  download_path = 'downloads'

  download_targets = []
  for k, v in instructions.items():
    t = os.path.join(download_path, v['target_directory'])
    download_targets.append(t)

  def python_download(instructions, targets):
    for k, instruction in instructions.items():
      if instruction['type'] == 'download':
        download_file(instruction, path='downloads')
      elif instruction['type'] == 'git-clone':
        git_clone(instruction, path='downloads')

  def python_extract(instructions, targets):
    for k, instruction in instructions.items():
      if instruction['extract'] is True:
        extract_file(instruction, path='downloads')

  return  {
    'actions' : [
      (python_download, [], {'instructions' : instructions}),
      (python_extract, [], {'instructions' : instructions})
    ],
    'file_dep' : ['download-instructions.yaml'],
    'targets' : download_targets
  }
  


@create_after(executed='construct_download_instructions', target_regex='capnp.yaml')
def task_build_capnp():        
  """Build capnp."""
  from doit.action import CmdAction
  import yaml, pathlib, os

  with open("capnp.yaml", 'r') as stream:
    instructions = yaml.load(stream)

  # create the build directory for cmake
  source_directory = os.path.join('downloads', instructions['target_directory'])
  target_directory = os.path.join('downloads', instructions['target_directory'] + "-build")
  depends_on = os.path.join('downloads', instructions['file_name'])
  library_directory = os.path.join(target_directory, 'src', 'capnp')
  target_binaries = {
    'capnp' : os.path.join(library_directory, 'capnp')
  }
  target_libraries = {
    'libcapnp.a' : os.path.join(library_directory, 'libcapnp.a'), 
    'libcapnpc.a' : os.path.join(library_directory, 'libcapnpc.a'), 
    'libcapnp-json.a' : os.path.join(library_directory, 'libcapnp-json.a'), 
    'libcapnp-rpc.a' : os.path.join(library_directory, 'libcapnp-rpc.a')
  }

  all_targets = {}
  all_targets.update(target_binaries)
  all_targets.update(target_libraries)

  with open("capnp-libraries.yaml", mode='w') as f:
    yaml.dump(target_libraries, f, default_flow_style=False, allow_unicode=True)

  if not pathlib.Path(target_directory).exists():
    pathlib.Path(target_directory).mkdir(parents=True, exist_ok=True) 

  return {
    'actions' : ['cmake -B{} -H{}'.format(target_directory, source_directory),
                 'make -C{}'.format(target_directory)],
    'targets' : [t for t in all_targets.values()] + ['capnp-libraries.yaml'],
    'file_dep' : [depends_on]
  }



@create_after(executed='construct_download_instructions', target_regex='capnp.yaml')
def task_prepare_capnp_schema():
  """Prepare capnp schema"""
  import yaml, os, pathlib, subprocess

  with open("capnp.yaml", 'r') as stream:
    instructions = yaml.load(stream)

  capnp_path = os.path.join('downloads', 
    instructions['target_directory'] + "-build",
    'src', 'capnp', 'capnp')

  schema_list = get_schema('pre-schema'); 
  targets = []
  depends = []
  for schema in schema_list:
    targets.append(os.path.join('schema', schema))
    depends.append(os.path.join('pre-schema', schema))

  def python_prepare_capnp_schema(capnp_path, targets):
  
    if not pathlib.Path('schema').exists():
      pathlib.Path('schema').mkdir(parents=True, exist_ok=True) 
  
    for f in schema_list:
      capnp_call = subprocess.run([capnp_path, 'id'], stdout=subprocess.PIPE)
      id_line = capnp_call.stdout.decode('utf8') + ";"
      with open(os.path.join('pre-schema', f), mode='r') as schema_f:
        schema = schema_f.read()
      with open(os.path.join('schema', f), mode='w') as schema_f:
        schema_f.write(id_line)
        schema_f.write(schema)

  return {
    'actions' : [(python_prepare_capnp_schema, [], {
        'capnp_path' : capnp_path  
      })],
    'targets' : targets,
    'file_dep' : depends
  }

@create_after(executed='construct_download_instructions', target_regex='capnp.yaml')
def task_compile_capnp_schema():
  """Use capnp compiler to generate c++ code"""
  import yaml, os, pathlib, subprocess

  with open("capnp.yaml", 'r') as stream:
    instructions = yaml.load(stream)

  capnp_path = os.path.join(
    os.path.abspath(os.curdir), 'downloads', 
    instructions['target_directory'] + "-build",
    'src', 'capnp')
  capnp_binary = os.path.join('downloads', 
    instructions['target_directory'] + "-build",
    'src', 'capnp', 'capnp')
  capnp_includes = os.path.join('downloads',
    instructions['target_directory'], 'src')
  o_dir = os.path.join('capnStan', 'src', 'capnp')

  schema_list = get_schema('pre-schema'); 
  targets = []
  depends = []
  actions = []
  for schema in schema_list:
    depends.append(os.path.join('schema', schema))
    targets.append(os.path.join(o_dir, schema + ".c++"))
    targets.append(os.path.join(o_dir, schema + ".h"))
    actions.append(
      'PATH=$PATH:' + capnp_path + ' ' + capnp_binary + ' ' +
      'compile' + ' ' + '-I' + capnp_includes + ' ' +
      '--src-prefix=schema' + ' ' +
      '-oc++:' + o_dir + ' ' + os.path.join('schema', schema)
    )

  if not pathlib.Path(o_dir).exists():
    pathlib.Path(o_dir).mkdir(parents=True, exist_ok=True) 

  return {
    'actions' : actions,
    'targets' : targets,
    'file_dep' : depends
  }


@create_after(executed='construct_download_instructions', target_regex='gtest.yaml')
def task_build_gtest():        
  """Build gtest."""
  from doit.action import CmdAction
  import yaml, pathlib, os

  with open("gtest.yaml", 'r') as stream:
    instructions = yaml.load(stream)

  # create the build directory for cmake
  source_directory = os.path.join('downloads', instructions['target_directory'])
  target_directory = os.path.join('downloads', instructions['target_directory'] + "-build")
  depends_on = os.path.join('downloads', instructions['file_name'])

  library_directory = os.path.join(target_directory, 'googlemock', 'gtest')
  target_libraries = {
    'libgtest.a' : os.path.join(library_directory, 'libgtest.a'), 
    'libgtest_main.a' : os.path.join(library_directory, 'libgtest_main.a')
  }

  with open("gtest-libraries.yaml", mode='w') as f:
    yaml.dump(target_libraries, f, default_flow_style=False, allow_unicode=True)

  if not pathlib.Path(target_directory).exists():
    pathlib.Path(target_directory).mkdir(parents=True, exist_ok=True) 

  return {
    'actions' : ['cmake -B{} -H{}'.format(target_directory, source_directory),
                 'make -C{}'.format(target_directory)],
    'targets' : [ v for v in target_libraries.values() ] + ['gtest-libraries.yaml'],
    'file_dep' : [depends_on]
  }


@create_after(executed='construct_download_instructions', target_regex='gtest.yaml')
def task_build_capnstan():        
  """Build capnstan."""
  from doit.action import CmdAction
  import yaml, pathlib, os

  # create the build directory for cmake
  source_directory = os.path.join('capnStan')
  target_directory = os.path.join('downloads', 'capnStan-test')
  test_target_directory = os.path.join(target_directory, 'test', 'unit')

  depends_on = []
  for root, dirs, files in os.walk(source_directory):
    for f in files:
      depends_on.append(os.path.join(root, f))

  with open("gtest-libraries.yaml", 'r') as stream:
    gtest_dependencies = yaml.load(stream)
    for dependency, path in gtest_dependencies.items():
      depends_on.append(path)

  if not pathlib.Path(target_directory).exists():
    pathlib.Path(target_directory).mkdir(parents=True, exist_ok=True) 

  targets_are = []
  for root, _, files in os.walk(source_directory):
    for f in files:
      if f.endswith('_test.cpp'):
        targets_are.append(os.path.abspath(os.path.join(root, f)))

  return {
    'actions' : ['cmake -B{} -H{}'.format(target_directory, source_directory),
                 'make -C{}'.format(target_directory),
                 'make -C{} test'.format(target_directory)
                 ],
    'targets' : targets_are,
    'file_dep' : depends_on
  }


