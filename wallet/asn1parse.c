/*
 *  Generic ASN.1 parsing
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#include "asn1.h"
#include "gMem.h"


#if(0)

#define NULL ((void*)0x00)

/* Implementation that should never be optimized out by the compiler */
static void zeroize( void *v, int n ) {
    volatile unsigned char *p = (unsigned char*)v; while( n-- ) *p++ = 0;
}


/*
 * ASN.1 DER decoding routines
 */
int asn1_get_len( unsigned char **p,
                  const unsigned char *end,
                  int *len )
{
    if( ( end - *p ) < 1 )
        return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

    if( ( **p & 0x80 ) == 0 )
        *len = *(*p)++;
    else
    {
        switch( **p & 0x7F )
        {
        case 1:
            if( ( end - *p ) < 2 )
                return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

            *len = (*p)[1];
            (*p) += 2;
            break;

        case 2:
            if( ( end - *p ) < 3 )
                return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

            *len = ( (int)(*p)[1] << 8 ) | (*p)[2];
            (*p) += 3;
            break;

        case 3:
            if( ( end - *p ) < 4 )
                return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

            *len = ( (int)(*p)[1] << 16 ) |
                   ( (int)(*p)[2] << 8  ) | (*p)[3];
            (*p) += 4;
            break;

        case 4:
            if( ( end - *p ) < 5 )
                return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

            *len = ( (int)(*p)[1] << 24 ) | ( (int)(*p)[2] << 16 ) |
                   ( (int)(*p)[3] << 8  ) |           (*p)[4];
            (*p) += 5;
            break;

        default:
            return( MBEDTLS_ERR_ASN1_INVALID_LENGTH );
        }
    }

    if( *len > (int) ( end - *p ) )
        return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

    return( 0 );
}

int asn1_get_tag( unsigned char **p,
                  const unsigned char *end,
                  int *len, int tag )
{
    if( ( end - *p ) < 1 )
        return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

    if( **p != tag )
        return( MBEDTLS_ERR_ASN1_UNEXPECTED_TAG );

    (*p)++;

    return( asn1_get_len( p, end, len ) );
}

				  

int asn1_get_bool( unsigned char **p,
                   const unsigned char *end,
                   int *val )
{
    int ret;
    int len;

    if( ( ret = asn1_get_tag( p, end, &len, MBEDTLS_ASN1_BOOLEAN ) ) != 0 )
        return( ret );

    if( len != 1 )
        return( MBEDTLS_ERR_ASN1_INVALID_LENGTH );

    *val = ( **p != 0 ) ? 1 : 0;
    (*p)++;

    return( 0 );
}

int asn1_get_int( unsigned char **p,
                  const unsigned char *end,
                  int *val )
{
    int ret;
    int len;

    if( ( ret = asn1_get_tag( p, end, &len, MBEDTLS_ASN1_INTEGER ) ) != 0 )
        return( ret );

    if( len == 0 || len > sizeof( int ) || ( **p & 0x80 ) != 0 )
        return( MBEDTLS_ERR_ASN1_INVALID_LENGTH );

    *val = 0;

    while( len-- > 0 )
    {
        *val = ( *val << 8 ) | **p;
        (*p)++;
    }

    return( 0 );
}


int asn1_get_bitstring( unsigned char **p, const unsigned char *end,
                        mbedtls_asn1_bitstring *bs)
{
    int ret;

    /* Certificate type is a single byte bitstring */
    if( ( ret = asn1_get_tag( p, end, &bs->len, MBEDTLS_ASN1_BIT_STRING ) ) != 0 )
        return( ret );

    /* Check length, subtract one for actual bit string length */
    if( bs->len < 1 )
        return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );
    bs->len -= 1;

    /* Get number of unused bits, ensure unused bits <= 7 */
    bs->unused_bits = **p;
    if( bs->unused_bits > 7 )
        return( MBEDTLS_ERR_ASN1_INVALID_LENGTH );
    (*p)++;

    /* Get actual bitstring */
    bs->p = *p;
    *p += bs->len;

    if( *p != end )
        return( MBEDTLS_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}

/*
 * Get a bit string without unused bits
 */
int asn1_get_bitstring_null( unsigned char **p, const unsigned char *end,
                             int *len )
{
    int ret;

    if( ( ret = asn1_get_tag( p, end, len, MBEDTLS_ASN1_BIT_STRING ) ) != 0 )
        return( ret );

    if( (*len)-- < 2 || *(*p)++ != 0 )
        return( MBEDTLS_ERR_ASN1_INVALID_DATA );

    return( 0 );
}



/*
 *  Parses and splits an ASN.1 "SEQUENCE OF <tag>"
 */
int asn1_get_sequence_of( unsigned char **p,
                          const unsigned char *end,
                          mbedtls_asn1_sequence *cur,
                          int tag)
{
    int ret;
    int len;
    mbedtls_asn1_buf *buf;

    /* Get main sequence tag */
    if( ( ret = asn1_get_tag( p, end, &len,
            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
        return( ret );

    if( *p + len != end )
        return( MBEDTLS_ERR_ASN1_LENGTH_MISMATCH );

    while( *p < end )
    {
        buf = &(cur->buf);
        buf->tag = **p;

        if( ( ret = asn1_get_tag( p, end, &buf->len, tag ) ) != 0 )
            return( ret );

        buf->p = *p;
        *p += buf->len;

        /* Allocate and assign next pointer */
        if( *p < end )
        {
            cur->next = (mbedtls_asn1_sequence*)gMalloc(sizeof( mbedtls_asn1_sequence ) );

            if( cur->next == NULL )
                return( MBEDTLS_ERR_ASN1_ALLOC_FAILED );
			
            cur = cur->next;
			zeroize(cur,sizeof( mbedtls_asn1_sequence ));
        }
    }

    /* Set final sequence entry's next pointer to NULL */
    cur->next = NULL;

    if( *p != end )
        return( MBEDTLS_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}

int asn1_get_alg( unsigned char **p,
                  const unsigned char *end,
                  mbedtls_asn1_buf *alg, mbedtls_asn1_buf *params )
{
    int ret;
    int len;

    if( ( ret = asn1_get_tag( p, end, &len,
            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
        return( ret );

    if( ( end - *p ) < 1 )
        return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

    alg->tag = **p;
    end = *p + len;

    if( ( ret = asn1_get_tag( p, end, &alg->len, MBEDTLS_ASN1_OID ) ) != 0 )
        return( ret );

    alg->p = *p;
    *p += alg->len;

    if( *p == end )
    {
        zeroize( params, sizeof(mbedtls_asn1_buf) );
        return( 0 );
    }

    params->tag = **p;
    (*p)++;

    if( ( ret = asn1_get_len( p, end, &params->len ) ) != 0 )
        return( ret );

    params->p = *p;
    *p += params->len;

    if( *p != end )
        return( MBEDTLS_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}

int asn1_get_alg_null( unsigned char **p,
                       const unsigned char *end,
                       mbedtls_asn1_buf *alg )
{
    int ret;
    mbedtls_asn1_buf params;

    memset( &params, 0, sizeof(mbedtls_asn1_buf) );

    if( ( ret = asn1_get_alg( p, end, alg, &params ) ) != 0 )
        return( ret );

    if( ( params.tag != MBEDTLS_ASN1_NULL && params.tag != 0 ) || params.len != 0 )
        return( MBEDTLS_ERR_ASN1_INVALID_DATA );

    return( 0 );
}

void asn1_free_named_data( mbedtls_asn1_named_data *cur )
{
    if( cur == NULL )
        return;

    gFree( cur->oid.p );
    gFree( cur->val.p );

    zeroize( cur, sizeof( mbedtls_asn1_named_data ) );
}

void asn1_free_named_data_list( mbedtls_asn1_named_data **head )
{
    mbedtls_asn1_named_data *cur;

    while( ( cur = *head ) != NULL )
    {
        *head = cur->next;
        asn1_free_named_data( cur );
        gFree( cur );
    }
}

mbedtls_asn1_named_data *asn1_find_named_data( mbedtls_asn1_named_data *list,
                                       const char *oid, int len )
{
    while( list != NULL )
    {
        if( list->oid.len == len &&
            memcmp( list->oid.p, oid, len ) == 0 )
        {
            break;
        }

        list = list->next;
    }

    return( list );
}
#endif
