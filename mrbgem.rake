MRuby::Gem::Specification.new('mruby-emv') do |spec|
  spec.license = 'CloudWalk Inc. All Rights Reserved'
  spec.authors = 'CloudWalk Inc.'
  spec.version = "0.1.0"

  spec.cc.include_paths << "#{build.root}/src"
  spec.rbfiles = Dir.glob("#{dir}/mrblib/*.rb")

  spec.add_dependency 'mruby-ingenico'
end
