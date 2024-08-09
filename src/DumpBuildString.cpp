//////////////////////////////////////////////////////////////////////////////
// This file is part of 'CpswUtils'.
// It is subject to the license terms in the LICENSE.txt file found in the 
// top-level directory of this distribution and at: 
//    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
// No part of 'CpswUtils', including this file, 
// may be copied, modified, propagated, or distributed except according to 
// the terms contained in the LICENSE.txt file.
//////////////////////////////////////////////////////////////////////////////
#include <cpsw_api_user.h>
#include <stdio.h>
#include <getopt.h>
#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>

#define BUILD_STAMP_NAME  "BuildStamp"
#define GIT_HASH_NAME     "GitHash"
#define IP_ADDRESS_NAME   "ipAddr"

static void
usage(const char *nm)
{
	fprintf(stdout, "usage: %s [-h] -a <ip_addr>\n", nm);
	fprintf(stdout, "           -h     : this message\n");
	fprintf(stdout, "           -a <ip>: use IP address <ip>\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "           Print build string of common-platform firmware\n");
}

static const char *yamlFmt =
	"#schemaversion 3.0.0                  \n"
	"root:                                 \n"
	"  class: NetIODev                     \n"
	"  %s: %s                              \n"
	"  children:                           \n"
	"    mmio:                             \n"
	"      class: MMIODev                  \n"
	"      size:  0x100000000              \n"
	"      at:                             \n"
	"        SRP:                          \n"
	"          protocolVersion: SRP_UDP_V2 \n"
	"        UDP:                          \n"
	"          port: 8192                  \n"
	"      children:                       \n"
	"        BuildStamp:                   \n"
	"          class:       IntField       \n"
	"          sizeBits:           8       \n"
	"          encoding:       ASCII       \n"
	"          mode:              RO       \n"
	"          at:                         \n"
	"            offset: 0x800             \n"
	"            nelms:  256               \n"
	"        GitHash:                      \n"
	"          class:       IntField       \n"
	"          sizeBits:           8       \n"
	"          mode:              RO       \n"
	"          at:                         \n"
	"            offset: 0x600             \n"
	"            nelms:  20                \n"
;

class Finder : public IPathVisitor {
private:
	Path        r;
	Path        p;
	const char *n;
public:
	Finder(Path root)
	: r( root ),
	  n( ""   )
	{
	}

	virtual bool visitPre(ConstPath here)
	{
		if ( here && 0 == strcmp(here->tail()->getName(), n) ) {
			p = here->clone();
			return false;
		}
		return true;
	}

	virtual void visitPost(ConstPath here)
	{
	}

	Path
	find(const char *what)
	{
		n = what;
		p = Path();
		r->explore( this );
		return p;
	}
};

#if 0
/* Once we use a later version of CPSW we can use the IYamlFixup::findByName()
 * to fixup the YAML (while traversing '<<' keys).
 */
class Fixup : public IYamlFixup {
const char *ip_;
public:
	Fixup(const char *ip)
	: ip_( ip )
	{
	}

	virtual void operator()(YAML::Node &root, YAML::Node &top)
	{
	YAML::Node ipNode = IYamlFixup::findByName( root, IP_ADDRESS_NAME );
		if ( ipNode.IsMap() ) {
			throw NotFoundError("'" IP_ADDRESS_NAME "' node not found in YAML\n");
		}
		ipNode = std::string( ip_ );
	}
};
#endif

int
main(int argc, char **argv)
{
const char *ip   = 0;
int         rval = 0;
int         opt;
unsigned    sz;
int64_t     got;

	while ( (opt = getopt(argc, argv, "ha:")) > 0 ) {
		switch ( opt ) {
			default:
				rval = 1;
			case 'h':
				usage( argv[0] );
				return rval;

			case 'a':
				ip = optarg;
			break;
		}
	}
	if ( ! ip ) {
		fprintf(stderr," -a <ip_addr> argument required!\n");
		return 1;
	}

	sz = strlen(yamlFmt) + 100;

	shared_ptr<char> strbuf( (char*) malloc( sz ) );

	if ( ! strbuf ) {
		fprintf(stderr, "ERROR: No memory\n");
		return 1;
	}

	snprintf( strbuf.get(), sz, yamlFmt, IP_ADDRESS_NAME, ip );

	Path r = IPath::loadYamlStream( strbuf.get(), "root" );

	Finder finder( r );

	Path found = finder.find( BUILD_STAMP_NAME );

	ScalVal_RO stamp;

	if ( found ) {
		stamp = IScalVal_RO::create( found );
	} else {
		fprintf( stderr, "ERROR: Unable to locate %s in YAML hierarchy\n", BUILD_STAMP_NAME );
		return 1;
	}

	got = stamp->getVal( (uint8_t*) strbuf.get(), sz );
	if ( got <= sz ) {
		strbuf.get()[got] = 0;
	}
	printf("%s\n", strbuf.get());

	found  = finder.find( GIT_HASH_NAME );

	ScalVal_RO hash;

	if ( found ) {
		hash = IScalVal_RO::create( found );
	} else {
		fprintf( stderr, "ERROR: Unable to locate %s in YAML hierarchy\n", GIT_HASH_NAME );
		return 1;
	}

	uint8_t gith[ hash->getNelms() ];

	hash->getVal( gith, hash->getNelms() );

	printf("Git Hash: ");

	for ( int i = hash->getNelms() - 1; i>=0; i-- ) {
		printf("%02x", gith[i]);
	}

	printf("\n");

	return 0;
}
