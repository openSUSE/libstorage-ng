# -*- coding: utf-8 -*-
# make continuous integration using rubygem-packaging_rake_tasks
# Copyright © 2016 SUSE
# MIT license

require "packaging/tasks"
require "packaging/configuration"
# skip 'tarball' task, it's redefined here
Packaging::Tasks.load_tasks(:exclude => ["tarball.rake"])

require "yast/tasks"
Yast::Tasks.submit_to(ENV.fetch("YAST_SUBMIT", "factory").to_sym)

Packaging.configuration do |conf|
  conf.package_dir    = ".obsdir" # Makefile.ci puts it there
  conf.skip_license_check << /.*/
  conf.package_name = "libstorage-ng"
  # The package does not live in the official YaST:Head OBS project
  conf.obs_project = "YaST:storage-ng"
  # Non-existent project to prevent accidental submission to Tumbleweed
  conf.obs_sr_project = "NONE"
end

desc 'Pretend to run the test suite'
task :test do
  puts 'No tests yet' if verbose
end

desc 'Build a tarball for OBS'
task :tarball do
  sh "make -f Makefile.ci package"
end
