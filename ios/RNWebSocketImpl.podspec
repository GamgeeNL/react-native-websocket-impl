
Pod::Spec.new do |s|
  s.name         = "RNWebSocketImpl"
  s.version      = "1.0.0"
  s.summary      = "RNWebSocketImpl"
  s.description  = <<-DESC
                  Custom implementation of WebSocket client for react-native
                   DESC
  s.homepage     = "https://github.com/Iskander508/react-native-websocket-impl"
  s.license      = "MIT"
  # s.license      = { :type => "MIT", :file => "FILE_LICENSE" }
  s.author             = { "author" => "author@domain.cn" }
  s.platform     = :ios, "9.0"
  s.source       = { :git => "https://github.com/Iskander508/react-native-websocket-impl.git", :tag => "master" }
  s.source_files  = "**/*.{h,m}"
  s.requires_arc = true


  s.dependency "React"
  #s.dependency "others"

end

  