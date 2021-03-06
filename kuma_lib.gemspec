# frozen_string_literal: true

require_relative "lib/kuma_lib/version"

Gem::Specification.new do |spec|
  spec.name          = "kuma_lib"
  spec.version       = KumaLib::VERSION
  spec.authors       = ["kumakumarb"]
  spec.email         = ["rinngo0119@gmail.com"]

  spec.summary       = "cpp std library and atcoder library."
  # spec.description   = "TODO: Write a longer description or delete this line."
  spec.homepage      = "https://github.com/kumakumarb/kuma_lib.git"
  spec.license       = "MIT"
  spec.required_ruby_version = Gem::Requirement.new(">= 2.4.0")

  # spec.metadata["allowed_push_host"] = "TODO: Set to 'http://mygemserver.com'"

  spec.metadata["homepage_uri"] = spec.homepage
  spec.metadata["source_code_uri"] = "https://github.com/kumakumarb/kuma_lib.git"
  spec.metadata["changelog_uri"] = "https://github.com/kumakumarb/kuma_lib.git"

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`.split("\x0").reject { |f| f.match(%r{\A(?:test|spec|features)/}) }
  end
  spec.bindir        = "exe"
  spec.executables   = spec.files.grep(%r{\Aexe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]
  spec.extensions    = %w[
    ext/kuma_lib/extconf.rb
    ext/priority_queue/extconf.rb
    ext/kuma_set/extconf.rb
    ext/kuma_sorted_set/extconf.rb
    ext/segment_tree/extconf.rb
  ]

  spec.files = %w[
    lib/kuma_lib/kuma_lib.bundle
    lib/kuma_lib/kuma_set.bundle
    lib/kuma_lib/kuma_set.bundle
    lib/kuma_lib/priority_queue.bundle
    lib/kuma_lib/segment_tree.bundle
    lib/kuma_lib/version.rb
    lib/kuma_lib.rb
  ]

  spec.add_development_dependency "rspec", "~> 3.0"

  # Uncomment to register a new dependency of your gem
  # spec.add_dependency "example-gem", "~> 1.0"

  # For more information and examples about making a new gem, checkout our
  # guide at: https://bundler.io/guides/creating_gem.html
end
