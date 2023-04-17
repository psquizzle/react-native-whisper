require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))
base_compiler_flags = '-DGGML_USE_ACCELERATE -Wno-shorten-64-to-32'
folly_compiler_flags = "-DFOLLY_NO_CONFIG -DFOLLY_MOBILE=1 -DFOLLY_USE_LIBCPP=1 -Wno-comma"
base_optimizer_flags = "-O3 -DNDEBUG"

Pod::Spec.new do |s|
  s.name         = "react-native-jsi-template"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.homepage     = package["homepage"]
  s.license      = package["license"]
  s.authors      = package["author"]

  s.platforms    = { :ios => "13.0" }
  s.source       = { :git => "https://github.com/ammarahm-ed/react-native-jsi-template.git", :tag => "#{s.version}" }

  s.source_files = "ios/**/*.{h,m,mm}", "cpp/**/*.{h,cpp,c}"

  s.dependency "React-Core"
  s.dependency 'ffmpeg-kit-ios-https', "5.1"
  s.ios.deployment_target = '12.1'

  s.compiler_flags = base_compiler_flags
  s.xcconfig = {
    "OTHER_LDFLAGS" => "-framework Accelerate",
    "OTHER_CFLAGS[config=Release]" => base_optimizer_flags,
    "OTHER_CPLUSPLUSFLAGS[config=Release]" => base_optimizer_flags
  }

end
 