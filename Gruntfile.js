module.exports = function(grunt) {
    "use strict";
    grunt.loadNpmTasks('grunt-contrib-compress');
    grunt.initConfig({
        compress: {
            main: {
                options: {
                    mode: 'gzip'
                },
                files: [{
                    expand: true,
                    cwd: 'dist/',
                    src: ['**/*.js'],
                    dest: 'data/',
                    ext: '.js.gz'
                },{
                    expand: true,
                    cwd: 'dist/',
                    src: ['**/*.htm'],
                    dest: 'data/',
                    ext: '.htm.gz'
                },{
                    expand: true,
                    cwd: 'dist/',
                    src: ['**/*.json'],
                    dest: 'data/',
                    ext: '.json.gz'
                },{
                    expand: true,
                    cwd: 'dist/',
                    src: ['**/*.view.xml'],
                    dest: 'data/',
                    ext: '.view.xml.gz'
                },{
                    expand: true,
                    cwd: 'dist/',
                    src: ['**/*.properties'],
                    dest: 'data/',
                    ext: '.properties.gz'
                },{
                    expand: true,
                    cwd: 'dist/',
                    src: ['**/*.css'],
                    dest: 'data/',
                    ext: '.css.gz'
                },{
                    expand: true,
                    cwd: 'dist/',
                    src: ['**/*.woff'],
                    dest: 'data/',
                    ext: '.woff.gz'
                },{
                    expand: true,
                    cwd: 'dist/',
                    src: ['**/*.woff2'],
                    dest: 'data/',
                    ext: '.woff2.gz'
                }]
            }
        }
    });
};
