const std = @import("std");
const fs = std.fs;
const io = std.io;
const tokenize = std.mem.tokenize;

pub fn main() anyerror!void {

    const r_stream = io.bufferedReader(std.io.getStdIn().reader()).reader();
    const w_stream = io.bufferedWriter(std.io.getStdOut().writer()).writer();

    var r_buffer: [512]u8 = undefined;

    var current_word: []const u8 = "0";
    var buffer_size: usize = 0;

    while(try r_stream.readUntilDelimiterOrEof(&r_buffer, '\n')) |line| {
        var iterator = tokenize(u8, line, " ");
        var word = iterator.next().?[0..];
        if (!std.mem.eql(u8, word, current_word)) {
            if (!std.mem.eql(u8, current_word, "0")) {
                buffer_size += try w_stream.write("\n");
            }
            buffer_size += try w_stream.write(word);
            current_word = word;
        }

        buffer_size += try w_stream.write(" ");
        var val_slice = iterator.next().?;
        var value = val_slice[0..val_slice.len - 1];
        buffer_size += try w_stream.write(value);

        if (buffer_size > 4000) {
            _ = try w_stream.context.flush();
            buffer_size = 0;
        }
    }
}