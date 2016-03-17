# -*- coding: utf-8 -*-
# make continuous integration using rubygem-packaging_rake_tasks
# Copyright © 2016 SUSE
# MIT license

require "packaging/tasks"
require "packaging/configuration"
# skip 'tarball' task because it's redefined here and 'check:changelog' task
# because it makes no sense at this stage of the development
Packaging::Tasks.load_tasks(:exclude => ["tarball.rake", "check_changelog.rake"])
Rake::Task["package"].prerequisites.delete("check:changelog")

require "yast/tasks"
Yast::Tasks.submit_to(ENV.fetch("YAST_SUBMIT", "factory").to_sym)

Packaging.configuration do |conf|
  conf.package_dir    = ".obsdir" # Makefile.ci puts it there
  # FIXME: we should not skip .c and .h files from the license check
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
