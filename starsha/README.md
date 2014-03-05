## starsha: The Stig build tool.

Starsha is used for building stig and its related tools and libraries.

This documentation is a work in progress. As we remember features, usage, etc. we will add them to this doc.

### If a starsha build fails

Sometimes your build will fail. Screws fall out. The world's an imperfect place. When the build fails, do the following to clean up before you start the build over again:

1. Start in the source directory (typically named `src`)
1. Delete the `out` directory. It will be one level above `src`: `rm -rf ../out`
1. Delete the starsha `.notes` file. This will be in the `.starsha` directory, one level about `src`: `rm -f ../.starsha/.notes`
1. Re-run the bootstrap script in order to rebuild nycr (which was in `out`): `./bootstrap.sh`
1. Rebuild the starsha `.notes` file: `starsha`
1. Make the necessary changes to fix whatever broke in the build
1. Build all the things: `starsha /stig/stig /stig/spa/spa /stig/server/stig /bruce/bruce /bruce/kafka_0_8_util/mdrequest /starsha/dummy` (the `/starsha/dummy` is needed for the starsha unit tests to work

-----

README.md Copyright 2010-2014 Stig LLC

README.md is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.

You should have received a copy of the license along with this work. If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.
