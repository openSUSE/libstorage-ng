# -*- coding: utf-8 -*-
# make continuous integration using rubygem-packaging_rake_tasks
# Copyright © 2016 SUSE
# MIT license

require "packaging/tasks"
require "packaging/configuration"
# Skip 'tarball' task because it's redefined here.
Packaging::Tasks.load_tasks(:exclude => ["tarball.rake"])

require "yast/tasks"
Yast::Tasks.submit_to(ENV.fetch("YAST_SUBMIT", "factory").to_sym)

Packaging.configuration do |conf|
  conf.package_dir    = ".obsdir" # Makefile.ci puts it there
  # FIXME: we should not skip .c and .h files from the license check
  conf.skip_license_check << /.*/
  conf.package_name = "libstorage-ng"
end

desc 'Pretend to run the test suite'
task :test do
  puts 'No tests yet' if verbose
end

desc 'Build a tarball for OBS'
task :tarball do
  sh "make -f Makefile.ci package"
end

task :update_package_location do
  # read the spec file from "package" dir if ".obsdir" does not exist yet
  if !File.exist?(".obsdir")
    Packaging.configuration { |conf| conf.package_dir = "package" }
  end
end

# update the "build_dependencies:*" tasks to read the alternative spec file
task :"build_dependencies:list" => :update_package_location
task :"build_dependencies:install" => :update_package_location
