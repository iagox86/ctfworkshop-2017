def p(file)
  File.open(file, 'rb') do |f|
    data = f.read()
    print data
  end
end

if(!ARGV[0])
  puts("Usage: ruby binary-to-string.rb <file[.asm]>")
  exit()
end

file = ARGV[0]

if(file.end_with?('.asm'))
  if(system("nasm -o #{file}.bin #{file}"))
    p(file + '.bin')
    File.delete(file + '.bin')
  end
else
  p(file)
end
