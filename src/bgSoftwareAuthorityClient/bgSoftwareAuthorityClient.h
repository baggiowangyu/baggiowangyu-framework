#ifndef _BG_SOFTWARE_AUTHORITY_CLIENT_H_
#define _BG_SOFTWARE_AUTHORITY_CLIENT_H_



class bgSoftwareAuthorityClient
{
public:
	bgSoftwareAuthorityClient();
	~bgSoftwareAuthorityClient();

public:
	int Authority();
	int Unauthority();

public:
	int GenerateAuthority();
	int CheckAuthority();
};

#endif
