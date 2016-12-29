const gulp = require('gulp');
const htmlmin = require('gulp-htmlmin');
const cleancss = require('gulp-clean-css');
const uglify = require('gulp-uglify');
const gzip = require('gulp-gzip');
const del = require('del');
const inline = require('gulp-inline');

/* Clean destination folder */
gulp.task('clean', function() {
  del(['data/*']);
  return true;
});

/* Copy static files */
gulp.task('files', function() {
  return gulp.src([
    'html/**/*.{jpg,jpeg,png,ico,gif}',
    'html/fsversion'
  ])
  .pipe(gulp.dest('data/'));
});

/* Process HTML, CSS, JS  --- INLINE --- */
gulp.task('inline', function() {
  return gulp.src('html/*.html')
    .pipe(inline({
      base: 'html/',
      js: uglify,
      css: cleancss,
      disabledTypes: ['svg', 'img']
    }))
    .pipe(htmlmin({
      collapseWhitespace: true,
      removeComments: true,
      minifyCSS: true,
      minifyJS: true
    }))
    .pipe(gulp.dest('.tmp'))
    .pipe(gzip())
    .pipe(gulp.dest('data'));
})

/* Build file system */
gulp.task('buildfs', ['clean', 'files', 'inline']);
gulp.task('default', ['buildfs']);
